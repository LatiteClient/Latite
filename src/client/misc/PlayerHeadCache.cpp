#include "pch.h"
#include "PlayerHeadCache.h"

#include "TempStorage.h"
#include "mc/common/world/actor/player/SerializedSkinRef.h"

#include <limits>

namespace {
	constexpr uint32_t playerHeadTextureSize = 64;

	uint64_t hashSkinBytes(SDK::SkinImage const& image, std::string const& id) {
		uint64_t hash = util::FNV_OFFSET_BASIS_64;
		auto mix = [&hash](uint8_t byte) {
			hash *= util::FNV_PRIME_64;
			hash ^= byte;
		};

		for (char ch : id) {
			mix(static_cast<uint8_t>(ch));
		}

		for (auto value : { image.width, image.height, static_cast<uint32_t>(image.bytes.size()) }) {
			for (int shift = 0; shift < 32; shift += 8) {
				mix(static_cast<uint8_t>((value >> shift) & 0xFF));
			}
		}

		auto data = image.bytes.data();
		if (data) {
			auto sampleSize = std::min<size_t>(image.bytes.size(), 4096);
			for (size_t i = 0; i < sampleSize; ++i) {
				mix(data[i]);
			}
		}

		return hash;
	}

	std::vector<uint8_t> makePlayerHeadRgba(SDK::SkinImage const& image) {
		if (!image.hasRgbaBytes()) return {};
		if (image.width < 64 || image.width % 8 != 0) return {};

		auto data = image.bytes.data();
		if (!data) return {};

		uint32_t sourceHeadSize = image.width / 8;
		uint32_t faceX = sourceHeadSize;
		uint32_t faceY = sourceHeadSize;
		uint32_t overlayX = sourceHeadSize * 5;
		if (overlayX + sourceHeadSize > image.width || faceY + sourceHeadSize > image.height) return {};

		std::vector<uint8_t> head(playerHeadTextureSize * playerHeadTextureSize * 4);
		auto pixel = [&](uint32_t x, uint32_t y) {
			return data + ((static_cast<size_t>(y) * image.width + x) * 4u);
		};

		for (uint32_t y = 0; y < playerHeadTextureSize; ++y) {
			uint32_t srcY = faceY + (y * sourceHeadSize / playerHeadTextureSize);
			for (uint32_t x = 0; x < playerHeadTextureSize; ++x) {
				uint32_t srcX = faceX + (x * sourceHeadSize / playerHeadTextureSize);
				uint32_t hatX = overlayX + (x * sourceHeadSize / playerHeadTextureSize);

				auto base = pixel(srcX, srcY);
				auto overlay = pixel(hatX, srcY);
				auto out = head.data() + ((static_cast<size_t>(y) * playerHeadTextureSize + x) * 4u);

				uint32_t overlayAlpha = overlay[3];
				uint32_t inverseAlpha = 255u - overlayAlpha;
				out[0] = static_cast<uint8_t>((overlay[0] * overlayAlpha + base[0] * inverseAlpha) / 255u);
				out[1] = static_cast<uint8_t>((overlay[1] * overlayAlpha + base[1] * inverseAlpha) / 255u);
				out[2] = static_cast<uint8_t>((overlay[2] * overlayAlpha + base[2] * inverseAlpha) / 255u);
				out[3] = 255;
			}
		}

		return head;
	}

	bool validateSkinImageForPlayerHead(SDK::SkinImage const* image) {
		if (image && image->hasRgbaBytes()) {
			return true;
		}

		if (!image || image->width == 0 || image->height == 0) {
			return false;
		}

		auto width = static_cast<size_t>(image->width);
		auto height = static_cast<size_t>(image->height);
		auto maxPixelCount = std::numeric_limits<size_t>::max() / 4u;
		auto pixelCount = width * height;
		bool pixelCountOverflow = height != 0 && pixelCount / height != width;
		bool suspiciousLayout =
			image->width > 4096 ||
			image->height > 4096 ||
			pixelCountOverflow ||
			pixelCount > maxPixelCount ||
			image->bytes.data() == nullptr ||
			image->bytes.size() < pixelCount * 4u;

		static bool warned = false;
		if (suspiciousLayout && !warned) {
			warned = true;
			Logger::Warn(
				"Player head skin layout validation failed. image=0x{:X}, format={}, width={}, height={}, bytes=0x{:X}, byteCount={}, expectedByteCount={}. Minecraft player skin layout may have changed.",
				reinterpret_cast<uintptr_t>(image),
				image->imageFormat,
				image->width,
				image->height,
				reinterpret_cast<uintptr_t>(image->bytes.data()),
				image->bytes.size(),
				pixelCountOverflow || pixelCount > maxPixelCount ? 0 : pixelCount * 4u
			);
		}

		return false;
	}

	bool writeRgbaPng(std::filesystem::path const& path, std::vector<uint8_t> const& rgba, uint32_t width, uint32_t height) {
		if (rgba.size() < static_cast<size_t>(width) * height * 4u) return false;

		std::error_code ec;
		std::filesystem::create_directories(path.parent_path(), ec);
		if (ec) return false;

		auto factory = Latite::getRenderer().getImagingFactory();
		if (!factory) return false;

		std::vector<uint8_t> bgra(rgba.size());
		for (size_t i = 0; i + 3 < rgba.size(); i += 4) {
			bgra[i] = rgba[i + 2];
			bgra[i + 1] = rgba[i + 1];
			bgra[i + 2] = rgba[i];
			bgra[i + 3] = rgba[i + 3];
		}

		auto tempPath = path;
		tempPath += ".tmp";
		std::filesystem::remove(tempPath, ec);

		auto encoded = [&]() -> bool {
			ComPtr<IWICStream> stream;
			if (FAILED(factory->CreateStream(stream.GetAddressOf()))) return false;
			if (FAILED(stream->InitializeFromFilename(tempPath.wstring().c_str(), GENERIC_WRITE))) return false;

			ComPtr<IWICBitmapEncoder> encoder;
			if (FAILED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.GetAddressOf()))) return false;
			if (FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache))) return false;

			ComPtr<IWICBitmapFrameEncode> frame;
			ComPtr<IPropertyBag2> properties;
			if (FAILED(encoder->CreateNewFrame(frame.GetAddressOf(), properties.GetAddressOf()))) return false;
			if (FAILED(frame->Initialize(properties.Get()))) return false;
			if (FAILED(frame->SetSize(width, height))) return false;

			WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
			if (FAILED(frame->SetPixelFormat(&format))) return false;
			if (!IsEqualGUID(format, GUID_WICPixelFormat32bppBGRA)) return false;

			uint32_t stride = width * 4u;
			if (FAILED(frame->WritePixels(height, stride, static_cast<UINT>(bgra.size()), bgra.data()))) return false;
			if (FAILED(frame->Commit())) return false;
			return SUCCEEDED(encoder->Commit());
		}();

		auto tempSize = std::filesystem::file_size(tempPath, ec);
		if (!encoded || ec || tempSize == 0) {
			std::filesystem::remove(tempPath, ec);
			return false;
		}

		std::filesystem::remove(path, ec);
		ec = {};
		std::filesystem::rename(tempPath, path, ec);
		if (ec) {
			std::filesystem::remove(tempPath, ec);
			return false;
		}

		auto size = std::filesystem::file_size(path, ec);
		return !ec && size > 0;
	}
}

std::string PlayerHeadCache::getTexturePath(SDK::SerializedSkinRef const& skin) {
	static std::unordered_map<uint64_t, std::string> cachedPaths;

	auto image = skin.getSkinImage();
	if (!validateSkinImageForPlayerHead(image)) return {};

	auto id = skin.getId();
	std::string key = id && !id->empty() ? *id : std::format("{}x{}:{}", image->width, image->height, image->bytes.size());
	uint64_t hash = hashSkinBytes(*image, key);

	if (auto found = cachedPaths.find(hash); found != cachedPaths.end()) {
		std::error_code ec;
		auto size = std::filesystem::file_size(found->second, ec);
		if (!ec && size > 0) {
			return found->second;
		}

		cachedPaths.erase(found);
	}

	auto head = makePlayerHeadRgba(*image);
	if (head.empty()) return {};

	auto directory = LatiteTemp::resolvePath("PlayerHeads");
	if (directory.empty()) return {};

	auto path = directory / std::format("head-{:016x}.png", hash);
	std::error_code ec;
	auto size = std::filesystem::file_size(path, ec);
	if (ec || size == 0) {
		ec = {};
		std::filesystem::remove(path, ec);
		if (!writeRgbaPng(path, head, playerHeadTextureSize, playerHeadTextureSize)) {
			std::filesystem::remove(path, ec);
			return {};
		}
	}

	std::string pathString = util::WStrToStr(path.wstring());
	cachedPaths[hash] = pathString;
	return pathString;
}

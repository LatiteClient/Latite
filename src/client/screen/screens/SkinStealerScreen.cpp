#include "pch.h"
#include "SkinStealerScreen.h"

#include "client/event/Eventing.h"
#include "client/event/events/ClickEvent.h"
#include "client/event/events/KeyUpdateEvent.h"
#include "client/event/events/RendererCleanupEvent.h"
#include "client/event/events/RenderOverlayEvent.h"
#include "client/Latite.h"
#include "client/localization/LocalizeString.h"
#include "client/render/asset/Assets.h"
#include "mc/common/world/actor/player/PlayerListEntry.h"
#include "mc/common/world/actor/player/SerializedSkinRef.h"
#include "mc/common/world/level/Level.h"
#include "util/DrawContext.h"

#include <Shellapi.h>
#include <algorithm>
#include <bit>
#include <limits>
#include <unordered_set>

SkinStealerScreen::SkinStealerScreen() {
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc) &SkinStealerScreen::onRender, 1, true);
	Eventing::get().listen<ClickEvent>(this, (EventListenerFunc) &SkinStealerScreen::onClick, 4);
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc) &SkinStealerScreen::onKey, 1);
	Eventing::get().listen<RendererCleanupEvent>(this, (EventListenerFunc) &SkinStealerScreen::onCleanup, 1, true);
}

namespace {
	constexpr float playerHeadTextureSize = 64.f;
	constexpr std::chrono::milliseconds feedbackDuration = std::chrono::milliseconds(1400);
}

void SkinStealerScreen::onEnable(bool) {
	rowsDirty = true;
	playerListSignature = 0;
	scroll = 0.f;
	lerpScroll = 0.f;
}

void SkinStealerScreen::onDisable() {
	mouseButtons = {};
	activeMouseButtons = {};
	justClicked = {};
	draggingScrollbar = false;
}

void SkinStealerScreen::onCleanup(Event &) {
	headBitmaps.clear();
}

void SkinStealerScreen::rebuildRows() {
	rows.clear();

	SDK::ClientInstance* client = SDK::ClientInstance::get();
	SDK::Level* level = client && client->minecraft ? client->minecraft->getLevel() : nullptr;
	auto playerList = level ? level->getPlayerList() : nullptr;
	if (!playerList) {
		playerListSignature = 0;
		rowsDirty = false;
		return;
	}

	rows.reserve(playerList->size());
	for (auto &item: *playerList) {
		SDK::PlayerListEntry &entry = item.second;
		SDK::SkinImage const *image = entry.skin.getSkinImage();
		if (!isValidSkinImage(image)) continue;

		PlayerRow row {};
		row.playerName = entry.name;
		row.displayName = util::StrToWStr(entry.name);
		row.skinKey = makeSkinKey(entry, *image);
		rows.push_back(std::move(row));
	}

	std::ranges::sort(rows, {}, &PlayerRow::playerName);
	pruneHeadCache();
	playerListSignature = getPlayerListSignature();
	rowsDirty = false;
}

uint64_t SkinStealerScreen::getPlayerListSignature() const {
	SDK::Level* level = SDK::ClientInstance::get()->minecraft->getLevel();
	auto* playerList = level ? level->getPlayerList() : nullptr;
	if (!playerList) return 0;

	uint64_t xorHash = 0;
	uint64_t sumHash = 0;
	uint64_t count = 0;

	for (auto &item: *playerList) {
		SDK::PlayerListEntry &entry = item.second;
		SDK::SkinImage const *image = entry.skin.getSkinImage();
		if (!isValidSkinImage(image)) continue;

		uint64_t entryHash = util::FNV_OFFSET_BASIS_64;
		for (char ch: entry.name) {
			entryHash *= util::FNV_PRIME_64;
			entryHash ^= static_cast<uint8_t>(ch);
		}
		if (std::string const *skinId = entry.skin.getId(); skinId && !skinId->empty()) {
			for (char ch: *skinId) {
				entryHash *= util::FNV_PRIME_64;
				entryHash ^= static_cast<uint8_t>(ch);
			}
		}
		for (uint64_t value: {
			     static_cast<uint64_t>(image->width),
			     static_cast<uint64_t>(image->height),
			     (image->bytes.size()),
			     reinterpret_cast<uintptr_t>(image->bytes.data())
		     }) {
			for (int shift = 0; shift < 64; shift += 8) {
				entryHash *= util::FNV_PRIME_64;
				entryHash ^= static_cast<uint8_t>((value >> shift) & 0xFF);
			}
		}

		xorHash ^= entryHash;
		sumHash += entryHash;
		++count;
	}

	return xorHash ^ std::rotl(sumHash, 17) ^ (count * util::FNV_PRIME_64);
}

void SkinStealerScreen::pruneHeadCache() {
	std::unordered_set<std::string> activeSkins;
	for (PlayerRow const &row: rows) {
		activeSkins.insert(row.skinKey);
	}

	for (std::unordered_map<std::string, HeadBitmap>::iterator it = headBitmaps.begin(); it != headBitmaps.end();) {
		if (activeSkins.contains(it->first)) ++it;
		else it = headBitmaps.erase(it);
	}
}

std::string SkinStealerScreen::makeSkinKey(SDK::PlayerListEntry &entry, SDK::SkinImage const &image) const {
	std::string id;
	if (std::string const *skinId = entry.skin.getId(); skinId && !skinId->empty()) {
		id = *skinId;
	} else {
		id = std::format("{}:{}x{}:{}", entry.name, image.width, image.height, image.bytes.size());
	}

	return std::format("{:016x}", hashSkin(image, id));
}

SkinStealerScreen::HeadBitmap *SkinStealerScreen::getHeadBitmap(SDK::PlayerListEntry &entry) {
	SDK::SkinImage const *image = entry.skin.getSkinImage();
	if (!isValidSkinImage(image)) return nullptr;

	std::string skinKey = makeSkinKey(entry, *image);
	if (std::unordered_map<std::string, HeadBitmap>::iterator found = headBitmaps.find(skinKey); found != headBitmaps.end() && found->second.bitmap) {
		return &found->second;
	}

	std::vector<uint8_t> rgba = makePlayerHeadRgba(*image);
	if (rgba.empty()) return nullptr;

	std::vector<uint8_t> bgra(rgba.size());
	for (size_t i = 0; i + 3 < rgba.size(); i += 4) {
		bgra[i] = rgba[i + 2];
		bgra[i + 1] = rgba[i + 1];
		bgra[i + 2] = rgba[i];
		bgra[i + 3] = rgba[i + 3];
	}

	D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

	HeadBitmap cached {};
	cached.skinKey = skinKey;
	ID2D1DeviceContext* dc = Latite::getRenderer().getDeviceContext();
	if (!dc || FAILED(dc->CreateBitmap(
		    D2D1::SizeU(static_cast<UINT32>(playerHeadTextureSize), static_cast<UINT32>(playerHeadTextureSize)),
		    bgra.data(),
		    static_cast<UINT32>(playerHeadTextureSize) * 4u,
		    &props,
		    cached.bitmap.GetAddressOf()))) {
		return nullptr;
	}

	std::pair<std::unordered_map<std::string, HeadBitmap>::iterator, bool> insertResult =
		headBitmaps.insert_or_assign(skinKey, std::move(cached));
	return &insertResult.first->second;
}

std::vector<uint8_t> SkinStealerScreen::makePlayerHeadRgba(SDK::SkinImage const &image) const {
	if (!isValidSkinImage(&image) || image.width < 64 || image.width % 8 != 0) return {};

	uint32_t sourceHeadSize = image.width / 8;
	uint32_t faceX = sourceHeadSize;
	uint32_t faceY = sourceHeadSize;
	uint32_t overlayX = sourceHeadSize * 5;
	if (overlayX + sourceHeadSize > image.width || faceY + sourceHeadSize > image.height) return {};

	unsigned char const *data = image.bytes.data();
	std::vector<uint8_t> head(
		static_cast<size_t>(playerHeadTextureSize) * static_cast<size_t>(playerHeadTextureSize) * 4u);
	for (uint32_t y = 0; y < static_cast<uint32_t>(playerHeadTextureSize); ++y) {
		uint32_t srcY = faceY + (y * sourceHeadSize / static_cast<uint32_t>(playerHeadTextureSize));
		for (uint32_t x = 0; x < static_cast<uint32_t>(playerHeadTextureSize); ++x) {
			uint32_t srcX = faceX + (x * sourceHeadSize / static_cast<uint32_t>(playerHeadTextureSize));
			uint32_t hatX = overlayX + (x * sourceHeadSize / static_cast<uint32_t>(playerHeadTextureSize));

			unsigned char const *base = data + ((static_cast<size_t>(srcY) * image.width + srcX) * 4u);
			unsigned char const *overlay = data + ((static_cast<size_t>(srcY) * image.width + hatX) * 4u);
			uint8_t *out = head.data() + ((static_cast<size_t>(y) * static_cast<size_t>(playerHeadTextureSize) + x) * 4u);

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

std::filesystem::path SkinStealerScreen::makeSkinOutputPath(std::string const &playerName,
                                                            SDK::SkinImage const &image) const {
	std::filesystem::path directory = util::GetLatitePath() / "SkinStealer";
	std::string baseName = sanitizeFileName(playerName);
	if (baseName.empty()) baseName = "player";

	std::string hash = std::format("{:016x}", hashSkin(image, playerName));
	std::string suffix = std::format("-{}x{}-{}.png", image.width, image.height, hash.substr(0, 8));
	return directory / util::StrToWStr(baseName + suffix);
}

std::string SkinStealerScreen::sanitizeFileName(std::string name) const {
	for (char &ch: name) {
		if (ch == '<' || ch == '>' || ch == ':' || ch == '"' || ch == '/' || ch == '\\' ||
		    ch == '|' || ch == '?' || ch == '*' || static_cast<unsigned char>(ch) < 0x20) {
			ch = '_';
		}
	}
	while (!name.empty() && (name.back() == '.' || name.back() == ' ')) {
		name.pop_back();
	}
	return name;
}

bool SkinStealerScreen::writeRgbaPng(std::filesystem::path const &path, SDK::SkinImage const &image) const {
	if (!isValidSkinImage(&image)) return false;

	std::error_code ec;
	std::filesystem::create_directories(path.parent_path(), ec);
	if (ec) return false;

	IWICImagingFactory2* factory = Latite::getRenderer().getImagingFactory();
	if (!factory) return false;

	uint32_t width = image.width;
	uint32_t height = image.height;
	unsigned long long pixelBytes = static_cast<size_t>(width) * height * 4u;
	std::vector<uint8_t> bgra(pixelBytes);
	unsigned char const *rgba = image.bytes.data();
	for (size_t i = 0; i + 3 < pixelBytes; i += 4) {
		bgra[i] = rgba[i + 2];
		bgra[i + 1] = rgba[i + 1];
		bgra[i + 2] = rgba[i];
		bgra[i + 3] = rgba[i + 3];
	}

	std::filesystem::path tempPath = path;
	tempPath += ".tmp";
	std::filesystem::remove(tempPath, ec);

	// Keep WIC COM wrappers scoped so their file handles are released before we rename the temp file.
	bool encoded = [&]() -> bool {
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
		if (FAILED(frame->SetPixelFormat(&format)) || !IsEqualGUID(format, GUID_WICPixelFormat32bppBGRA)) return false;

		unsigned stride = width * 4u;
		if (FAILED(frame->WritePixels(height, stride, static_cast<UINT>(bgra.size()), bgra.data()))) return false;
		if (FAILED(frame->Commit())) return false;
		return SUCCEEDED(encoder->Commit());
	}();

	uintmax_t tempSize = std::filesystem::file_size(tempPath, ec);
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

	uintmax_t size = std::filesystem::file_size(path, ec);
	return !ec && size > 0;
}

std::optional<std::filesystem::path> SkinStealerScreen::saveSkin(SDK::PlayerListEntry &entry) {
	SDK::SkinImage const *image = entry.skin.getSkinImage();
	if (!isValidSkinImage(image)) return std::nullopt;

	std::filesystem::path path = makeSkinOutputPath(entry.name, *image);
	if (!writeRgbaPng(path, *image)) return std::nullopt;
	return path;
}

bool SkinStealerScreen::isValidSkinImage(SDK::SkinImage const *image) const {
	if (!image || !image->hasRgbaBytes()) return false;

	size_t width = image->width;
	size_t height = image->height;

	if (height != 0 && width > std::numeric_limits<size_t>::max() / height) return false;
	unsigned long long pixelCount = width * height;

	return pixelCount <= (std::numeric_limits<size_t>::max() / 4u)
		   && image->bytes.size() >= pixelCount * 4u;
}

uint64_t SkinStealerScreen::hashSkin(SDK::SkinImage const &image, std::string const &id) const {
	uint64_t hash = util::FNV_OFFSET_BASIS_64;

	for (char ch: id) {
		hash *= util::FNV_PRIME_64;
		hash ^= static_cast<uint8_t>(ch);
	}
	for (unsigned value: { image.width, image.height, static_cast<uint32_t>(image.bytes.size()) }) {
		for (int shift = 0; shift < 32; shift += 8) {
			hash *= util::FNV_PRIME_64;
			hash ^= static_cast<uint8_t>((value >> shift) & 0xFF);
		}
	}

	unsigned char const *data = image.bytes.data();
	size_t sampleSize = std::min<size_t>(image.bytes.size(), 4096);
	for (size_t i = 0; data && i < sampleSize; ++i) {
		hash *= util::FNV_PRIME_64;
		hash ^= data[i];
	}

	return hash;
}

void SkinStealerScreen::openSkinStealerFolder() {
	std::filesystem::path directory = util::GetLatitePath() / "SkinStealer";

	std::error_code ec;
	std::filesystem::create_directories(directory, ec);
	if (ec) return;

	ShellExecuteW(nullptr, L"open", directory.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void SkinStealerScreen::updateScrollbarDrag(Vec2 const &mouse) {
	if (!draggingScrollbar) return;
	if (!mouseButtons[0] || scrollMax <= 0.f || scrollbarTrackRect.getHeight() <= scrollbarThumbRect.getHeight()) {
		draggingScrollbar = false;
		return;
	}

	float availableTrack = scrollbarTrackRect.getHeight() - scrollbarThumbRect.getHeight();
	float thumbTop = std::clamp(mouse.y - scrollbarDragOffset, scrollbarTrackRect.top,
	                            scrollbarTrackRect.bottom - scrollbarThumbRect.getHeight());
	float normalized = (thumbTop - scrollbarTrackRect.top) / availableTrack;
	scroll = std::clamp(normalized * scrollMax, 0.f, scrollMax);
}

void SkinStealerScreen::onRender(Event &) {
	if (!isActive()) {
		justClicked = {};
		return;
	}

	if (rowsDirty || getPlayerListSignature() != playerListSignature) rebuildRows();

	D2DUtil dc;
	D2D1_SIZE_F screenSize = Latite::getRenderer().getScreenSize();
	Vec2 cursorPos = SDK::ClientInstance::get()->cursorPos;
	d2d::Color accent = d2d::Color(Latite::get().getAccentColor().getMainColor());

	updateScrollbarDrag(cursorPos);

	if (Latite::get().getMenuBlur()) {
		dc.drawGaussianBlur(Latite::get().getMenuBlur().value());
	}

	float scale = std::clamp(screenSize.width / 1920.f, 0.7f, 1.1f);
	float panelWidth = std::min(screenSize.width * 0.78f, 840.f * scale);
	float panelHeight = std::min(screenSize.height * 0.78f, 640.f * scale);
	panelRect = {
		(screenSize.width - panelWidth) * 0.5f,
		(screenSize.height - panelHeight) * 0.5f,
		(screenSize.width + panelWidth) * 0.5f,
		(screenSize.height + panelHeight) * 0.5f
	};

	float pad = 25.f * scale;
	float titleSize = 25.f * scale;
	float subSize = 15.f * scale;
	float rowHeight = 68.f * scale;
	float gap = 8.f * scale;
	float radius = 19.f * scale;
	float headerHeight = 70.f * scale;

	d2d::Color panelColor = d2d::Color::RGB(0x07, 0x07, 0x07).asAlpha(0.75f);
	d2d::Color outlineColor = d2d::Color::RGB(0x00, 0x00, 0x00).asAlpha(0.28f);
	dc.fillRoundedRectangle(panelRect, panelColor, radius);
	dc.drawRoundedRectangle(panelRect, outlineColor, radius, 4.f * scale, DrawUtil::OutlinePosition::Outside);

	float closeSize = 22.f * scale;
	closeButtonRect = {
		panelRect.right - pad - closeSize,
		panelRect.top + 28.f * scale,
		panelRect.right - pad,
		panelRect.top + 28.f * scale + closeSize
	};

	float folderButtonWidth = 118.f * scale;
	float folderButtonHeight = 32.f * scale;
	folderButtonRect = {
		closeButtonRect.left - 18.f * scale - folderButtonWidth,
		panelRect.top + 23.f * scale,
		closeButtonRect.left - 18.f * scale,
		panelRect.top + 23.f * scale + folderButtonHeight
	};

	float titleTop = panelRect.top + 19.f * scale;
	dc.drawText({ panelRect.left + pad, titleTop, folderButtonRect.left - 16.f * scale, titleTop + 31.f * scale },
	            LocalizeString::get("client.module.skinStealer.name"), d2d::Colors::WHITE, Renderer::FontSelection::PrimaryLight, titleSize,
	            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, false);
	dc.drawText({
		            panelRect.left + pad + 1.f * scale, titleTop + 30.f * scale, folderButtonRect.left - 16.f * scale,
		            titleTop + 54.f * scale
	            },
	            util::FormatWString(LocalizeString::get(rows.size() == 1
		            ? "client.screen.skinStealer.playerCount.one"
		            : "client.screen.skinStealer.playerCount.many"), { std::to_wstring(rows.size()) }),
	            d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.72f), Renderer::FontSelection::PrimaryRegular, subSize);

	bool folderHovered = folderButtonRect.contains(cursorPos);
	dc.fillRoundedRectangle(folderButtonRect, folderHovered ? accent : d2d::Color::RGB(0x38, 0x38, 0x38).asAlpha(0.88f),
	                        folderButtonRect.getHeight() * 0.23f);
	dc.drawText(folderButtonRect, LocalizeString::get("client.screen.skinStealer.openFolder.name"), d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
	            15.f * scale,
	            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	if (ID2D1Bitmap* closeIcon = Latite::getAssets().xIcon.getBitmap()) {
		dc.ctx->DrawBitmap(closeIcon, closeButtonRect, closeButtonRect.contains(cursorPos) ? 1.f : 0.72f);
	}

	listRect = {
		panelRect.left + pad,
		panelRect.top + headerHeight + 7.f * scale,
		panelRect.right - pad,
		panelRect.bottom - pad
	};

	float contentHeight = rows.empty() ? 0.f : static_cast<float>(rows.size()) * (rowHeight + gap) - gap;
	scrollMax = std::max(0.f, contentHeight - listRect.getHeight());
	scroll = std::clamp(scroll, 0.f, scrollMax);
	lerpScroll = std::lerp(lerpScroll, scroll, Latite::getRenderer().getDeltaTime() * 0.25f);
	if (std::abs(lerpScroll - scroll) < 0.1f) lerpScroll = scroll;

	if (rows.empty()) {
		dc.drawText(listRect, LocalizeString::get("client.screen.skinStealer.empty.name"), d2d::Color::RGB(0xA8, 0xB4, 0xC8),
		            Renderer::FontSelection::PrimaryRegular, 18.f * scale, DWRITE_TEXT_ALIGNMENT_CENTER,
		            DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		return;
	}

	dc.ctx->PushAxisAlignedClip(dc.getRect(listRect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	SDK::Level* level = SDK::ClientInstance::get()->minecraft->getLevel();
	auto* playerList = level ? level->getPlayerList() : nullptr;

	for (size_t i = 0; i < rows.size(); i++) {
		PlayerRow &row = rows[i];
		float y = listRect.top + (static_cast<float>(i) * (rowHeight + gap)) - lerpScroll;
		d2d::Rect rowRect { listRect.left, y, listRect.right, y + rowHeight };
		if (rowRect.bottom < listRect.top || rowRect.top > listRect.bottom) continue;

		bool hovered = rowRect.contains(cursorPos);
		dc.fillRoundedRectangle(rowRect, hovered
			                                 ? d2d::Color::RGB(0x2A, 0x2A, 0x2A).asAlpha(0.58f)
			                                 : d2d::Color::RGB(0x12, 0x12, 0x12).asAlpha(0.48f), 10.f * scale);

		float headSize = rowHeight - 20.f * scale;
		d2d::Rect headRect {
			rowRect.left + 12.f * scale, rowRect.top + 10.f * scale, rowRect.left + 12.f * scale + headSize,
			rowRect.top + 10.f * scale + headSize
		};
		dc.fillRoundedRectangle(headRect, d2d::Color::RGB(0x00, 0x00, 0x00).asAlpha(0.38f), 7.f * scale);

		SDK::PlayerListEntry *entry = nullptr;
		if (playerList) {
			for (auto &item: *playerList) {
				if (item.second.name == row.playerName) {
					entry = &item.second;
					break;
				}
			}
		}

		if (entry) {
			if (HeadBitmap *head = getHeadBitmap(*entry); head && head->bitmap) {
				dc.ctx->DrawBitmap(head->bitmap.Get(), dc.getRect(headRect), 1.f,
				                   D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}

		float textLeft = headRect.right + 14.f * scale;
		float buttonWidth = 86.f * scale;
		float buttonHeight = 32.f * scale;
		row.getButtonRect = {
			rowRect.right - 14.f * scale - buttonWidth,
			rowRect.top + (rowHeight - buttonHeight) * 0.5f,
			rowRect.right - 14.f * scale,
			rowRect.top + (rowHeight + buttonHeight) * 0.5f
		};

		d2d::Rect nameRect {
			textLeft, rowRect.top + 14.f * scale, row.getButtonRect.left - 12.f * scale, rowRect.top + 39.f * scale
		};
		dc.drawSingleLineFitted(nameRect, row.displayName, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
		                        20.f * scale);
		if (!row.status.empty()) {
			dc.drawSingleLineFitted({
				                        textLeft, rowRect.top + 41.f * scale, row.getButtonRect.left - 12.f * scale,
				                        rowRect.bottom - 8.f * scale
			                        },
			                        row.status, d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.68f),
			                        Renderer::FontSelection::PrimaryRegular, 13.f * scale);
		}

		bool buttonHovered = row.getButtonRect.contains(cursorPos);
		bool showingFeedback = std::chrono::steady_clock::now() < row.feedbackUntil;

		d2d::Color buttonColor = d2d::Color::RGB(0x38, 0x38, 0x38).asAlpha(0.88f);
		if (showingFeedback) {
			buttonColor = row.statusSuccess
				              ? d2d::Color::RGB(0x4C, 0xAF, 0x50)
				              : d2d::Color::RGB(0xF4, 0x43, 0x36);
		} else if (buttonHovered) {
			buttonColor = accent;
		}

		dc.fillRoundedRectangle(row.getButtonRect, buttonColor, row.getButtonRect.getHeight() * 0.23f);
		dc.drawText(row.getButtonRect, LocalizeString::get("client.screen.skinStealer.get.name"), d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
		            16.f * scale,
		            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	dc.ctx->PopAxisAlignedClip();

	if (scrollMax > 0.f) {
		float trackWidth = 4.f * scale;
		d2d::Rect track {
			listRect.right + 9.f * scale, listRect.top, listRect.right + 9.f * scale + trackWidth, listRect.bottom
		};
		float thumbHeight = std::max(24.f * scale,
		                             listRect.getHeight() * (
			                             listRect.getHeight() / (listRect.getHeight() + scrollMax)));
		float thumbY = track.top + ((track.getHeight() - thumbHeight) * (scroll / scrollMax));
		scrollbarTrackRect = track;
		scrollbarThumbRect = { track.left, thumbY, track.right, thumbY + thumbHeight };
		dc.fillRoundedRectangle(scrollbarTrackRect, d2d::Color::RGB(0x55, 0x55, 0x55).asAlpha(0.28f),
		                        trackWidth * 0.5f);
		dc.fillRoundedRectangle(scrollbarThumbRect,
		                        (draggingScrollbar || scrollbarThumbRect.contains(cursorPos))
			                        ? accent
			                        : d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.78f), trackWidth * 0.5f);
	} else {
		scrollbarTrackRect = {};
		scrollbarThumbRect = {};
	}
}

void SkinStealerScreen::onClick(Event &evGeneric) {
	ClickEvent &ev = reinterpret_cast<ClickEvent&>(evGeneric);
	if (!isActive()) return;

	ClickEvent::ClickType clickType = ev.getClickType();
	if (clickType != ClickEvent::ClickType::None) ev.setCancelled(true);

	if (clickType == ClickEvent::ClickType::Wheel) {
		scroll = std::clamp(scroll - static_cast<float>(ev.getWheelDelta()) / 3.f, 0.f, scrollMax);
		return;
	}

	if (clickType != ClickEvent::ClickType::Left) return;

	Vec2 cursorPos = SDK::ClientInstance::get()->cursorPos;
	mouseButtons[0] = ev.isDown();
	if (!ev.isDown()) {
		draggingScrollbar = false;
		return;
	}

	if (!panelRect.contains(cursorPos)) {
		close();
		return;
	}

	if (closeButtonRect.contains(cursorPos)) {
		playClickSound();
		close();
		return;
	}

	if (folderButtonRect.contains(cursorPos)) {
		playClickSound();
		openSkinStealerFolder();
		return;
	}

	if (scrollMax > 0.f && scrollbarTrackRect.contains(cursorPos)) {
		draggingScrollbar = true;
		if (scrollbarThumbRect.contains(cursorPos)) {
			scrollbarDragOffset = cursorPos.y - scrollbarThumbRect.top;
		} else {
			scrollbarDragOffset = scrollbarThumbRect.getHeight() * 0.5f;
			updateScrollbarDrag(cursorPos);
		}
		return;
	}

	if (!listRect.contains(cursorPos)) return;

	SDK::Level* level = SDK::ClientInstance::get()->minecraft->getLevel();
	auto playerList = level ? level->getPlayerList() : nullptr;
	if (!playerList) return;

	for (PlayerRow &row: rows) {
		if (!row.getButtonRect.contains(cursorPos)) continue;

		for (auto &item: *playerList) {
			if (item.second.name != row.playerName) continue;
			if (std::optional<std::filesystem::path> savedPath = saveSkin(item.second)) {
				row.status = util::FormatWString(LocalizeString::get("client.screen.skinStealer.saved.name"),
					{ savedPath->filename().wstring() });
				row.statusSuccess = true;
				row.feedbackUntil = std::chrono::steady_clock::now() + feedbackDuration;
				playClickSound();
			} else {
				row.status = LocalizeString::get("client.screen.skinStealer.saveError.name");
				row.statusSuccess = false;
				row.feedbackUntil = std::chrono::steady_clock::now() + feedbackDuration;
			}
			return;
		}
	}
}

void SkinStealerScreen::onKey(Event &evGeneric) {
	KeyUpdateEvent &ev = reinterpret_cast<KeyUpdateEvent &>(evGeneric);
	if (!isActive()) return;
	if (ev.getKey() == VK_F11) return;

	if (ev.isDown() && ev.getKey() == VK_ESCAPE) {
		close();
	}

	ev.setCancelled(true);
}

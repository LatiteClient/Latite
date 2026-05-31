#include "pch.h"
#include "WAILA.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <format>
#include <iomanip>
#include <limits>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <Shlwapi.h>

#include "mc/common/entity/component/ActorTypeComponent.h"
#include "mc/common/client/renderer/block/BlockGraphics.h"
#include "mc/common/world/ItemStack.h"
#include "mc/common/world/actor/item/ItemActor.h"
#include "mc/common/world/actor/player/Player.h"
#include "mc/common/world/level/BlockSource.h"
#include "mc/common/world/level/HitResult.h"
#include "mc/common/world/level/block/Block.h"
#include "mc/common/world/level/block/BlockLegacy.h"
#include "util/Logger.h"

namespace {
	constexpr float defaultWidth = 196.f;
	constexpr float defaultHeight = 52.f;
	constexpr float iconSize = 32.f;
	constexpr float paddingX = 9.f;
	constexpr float paddingY = 7.f;
	constexpr float textGap = 8.f;

	float vecLength(Vec3 const& vec) {
		return std::sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
	}

	float distance(Vec3 const& a, Vec3 const& b) {
		return vecLength(a - b);
	}

	Vec3 normalize(Vec3 vec) {
		float len = vecLength(vec);
		if (len <= 0.0001f) return {};
		return { vec.x / len, vec.y / len, vec.z / len };
	}

	std::string stripIdentifier(std::string id) {
		if (auto colon = id.find(':'); colon != std::string::npos) {
			id = id.substr(colon + 1);
		}
		if (id.starts_with("tile.")) {
			id = id.substr(5);
		}
		if (id.starts_with("item.")) {
			id = id.substr(5);
		}
		if (id.ends_with(".name")) {
			id.resize(id.size() - 5);
		}
		return id;
	}

	std::wstring widen(std::string_view value) {
		return util::StrToWStr(std::string(value));
	}

	std::wstring titleCaseIdentifier(std::string id) {
		id = stripIdentifier(std::move(id));
		std::wstring out;
		out.reserve(id.size());

		bool newWord = true;
		for (char ch : id) {
			if (ch == '_' || ch == '-' || ch == '.') {
				out.push_back(L' ');
				newWord = true;
				continue;
			}

			unsigned char uch = static_cast<unsigned char>(ch);
			if (newWord) {
				out.push_back(static_cast<wchar_t>(std::toupper(uch)));
			}
			else {
				out.push_back(static_cast<wchar_t>(std::tolower(uch)));
			}
			newWord = false;
		}

		if (out == L"Tnt") return L"TNT";
		if (out.empty()) return L"Unknown";
		return out;
	}

	std::string namespaceFrom(std::string id) {
		if (id.starts_with("tile.")) {
			id = id.substr(5);
		}
		if (id.starts_with("item.")) {
			id = id.substr(5);
		}
		if (auto colon = id.find(':'); colon != std::string::npos && colon > 0) {
			return id.substr(0, colon);
		}
		return "minecraft";
	}

	std::wstring formatBlockPos(BlockPos const& pos) {
		std::wstringstream ss;
		ss << pos.x << L", " << pos.y << L", " << pos.z;
		return ss.str();
	}

	std::wstring appendDetail(std::wstring base, std::wstring const& suffix) {
		if (suffix.empty()) return base;
		if (base.empty()) return suffix;
		return base + L"  " + suffix;
	}

	bool endsWith(std::string_view value, std::string_view suffix) {
		return value.size() >= suffix.size() && value.substr(value.size() - suffix.size()) == suffix;
	}

	std::string normalizeTexturePath(std::string path) {
		std::replace(path.begin(), path.end(), '\\', '/');
		if (endsWith(path, ".png")) {
			path.resize(path.size() - 4);
		}
		if (path.starts_with("blocks/") || path.starts_with("items/")) {
			path = "textures/" + path;
		}
		return path;
	}

	template <typename... Args>
	void logWailaOnce(std::string key, std::string_view fmt, Args&&... args) {
		static std::unordered_set<std::string> logged;
		if (!logged.emplace(std::move(key)).second) return;

		Logger::Info("[WAILA] {}", std::vformat(fmt, std::make_format_args(args...)));
	}

	bool hasPngSignature(std::string const& buffer) {
		constexpr std::array<unsigned char, 8> pngSignature{ 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
		if (buffer.size() < pngSignature.size()) return false;

		for (std::size_t i = 0; i < pngSignature.size(); ++i) {
			if (static_cast<unsigned char>(buffer[i]) != pngSignature[i]) {
				return false;
			}
		}
		return true;
	}

	bool loadPngTextureResource(std::string const& normalizedPath, std::string* outBuffer = nullptr) {
		if (normalizedPath.empty()) return false;

		for (auto const& candidate : std::array<std::string, 2>{ normalizedPath + ".png", normalizedPath }) {
			std::string buffer;
			if (!util::TryGetGameTextureBuffer(candidate, buffer)) {
				continue;
			}

			if (!hasPngSignature(buffer)) {
				logWailaOnce("texture-resource-not-png:" + candidate,
					"ResourcePackManager loaded '{}', but the buffer is not a PNG", candidate);
				continue;
			}

			if (outBuffer) {
				*outBuffer = std::move(buffer);
			}
			logWailaOnce("texture-resource:" + normalizedPath,
				"ResourcePackManager proved PNG '{}' via '{}'", normalizedPath, candidate);
			return true;
		}

		logWailaOnce("texture-resource:" + normalizedPath,
			"ResourcePackManager missed PNG '{}'", normalizedPath);
		return false;
	}

	bool textureResourceExists(std::string path) {
		path = normalizeTexturePath(std::move(path));
		if (path.empty()) return false;

		static std::unordered_map<std::string, bool> cache;
		if (auto found = cache.find(path); found != cache.end()) {
			return found->second;
		}

		auto exists = loadPngTextureResource(path);

		cache[path] = exists;
		return exists;
	}

	bool isMissingUv(SDK::TextureUVCoordinateSet const* uv) {
		if (!uv) return true;
		if (uv->sourceFileLocation.mPath->value.empty()) return true;
		if (uv->u1 <= uv->u0 || uv->v1 <= uv->v0) return true;

		auto path = uv->sourceFileLocation.mPath->value;
		std::transform(path.begin(), path.end(), path.begin(),
			[](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
		return path.find("missing") != std::string::npos;
	}

	SDK::TextureUVCoordinateSet const* previewOakLogUv() {
		static SDK::TextureUVCoordinateSet uv = [] {
			SDK::TextureUVCoordinateSet value{};
			value.weight = 1.f;
			value.u1 = 1.f;
			value.v1 = 1.f;
			value.sourceFileLocation = SDK::ResourceLocation("textures/blocks/log_oak", SDK::ResourceFileSystem::UserPackage);
			return value;
		}();
		return &uv;
	}

	SDK::TextureUVCoordinateSet const* resolveBlockTextureUv(SDK::Block const& block, BlockPos const& pos,
		std::string const& blockId) {
		constexpr std::uint64_t primaryTextureSlot = 0;

		auto graphics = SDK::BlockGraphics::getForBlock(block);
		if (!graphics) {
			logWailaOnce("block-graphics-null:" + blockId,
				"BlockGraphics::getForBlock returned null for '{}'", blockId);
			return nullptr;
		}

		auto uv = graphics->getTextureAt(pos, primaryTextureSlot, 0);
		if (isMissingUv(uv)) {
			auto path = uv ? uv->sourceFileLocation.mPath->value : std::string{};
			logWailaOnce("block-uv-missing:" + blockId,
				"BlockGraphics primary UV missing for '{}' slot {} path='{}' uv=({}, {}, {}, {})",
				blockId, primaryTextureSlot, path,
				uv ? uv->u0 : 0.f, uv ? uv->v0 : 0.f, uv ? uv->u1 : 0.f, uv ? uv->v1 : 0.f);
			return nullptr;
		}

		logWailaOnce("block-uv:" + blockId,
			"BlockGraphics primary UV for '{}' slot {} path='{}' fs={} uv=({}, {}, {}, {})",
			blockId, primaryTextureSlot, uv->sourceFileLocation.mPath->value,
			static_cast<int>(uv->sourceFileLocation.mFileSystem), uv->u0, uv->v0, uv->u1, uv->v1);
		return uv;
	}

	bool drawTextureUvIcon(MCDrawUtil& dc, SDK::TextureUVCoordinateSet const* uv, d2d::Rect const& icon) {
		if (isMissingUv(uv)) return false;

		SDK::TexturePtr texture{};
		dc.renderCtx->getTexture(&texture, uv->sourceFileLocation, false);
		if (!texture.textureData) {
			logWailaOnce("block-uv-texture-null:" + uv->sourceFileLocation.mPath->value,
				"getTexture returned null for BlockGraphics path '{}' fs={}",
				uv->sourceFileLocation.mPath->value, static_cast<int>(uv->sourceFileLocation.mFileSystem));
			return false;
		}

		dc.drawImage(texture, icon.getPos(), { icon.getWidth(), icon.getHeight() }, d2d::Colors::WHITE);
		logWailaOnce("block-uv-draw:" + uv->sourceFileLocation.mPath->value,
			"Drawing source texture '{}' fs={} at full UVs; metadata uv=({}, {}, {}, {})",
			uv->sourceFileLocation.mPath->value, static_cast<int>(uv->sourceFileLocation.mFileSystem),
			uv->u0, uv->v0, uv->u1, uv->v1);
		return true;
	}

	bool drawTextureIcon(MCDrawUtil& dc, std::string const& texturePath, d2d::Rect const& icon) {
		auto path = normalizeTexturePath(texturePath);
		if (path.empty()) return false;
		if (!textureResourceExists(path)) {
			logWailaOnce("texture-icon-missing:" + path,
				"Texture icon '{}' did not pass PNG proof", path);
			return false;
		}

		for (auto fileSystem : { SDK::ResourceFileSystem::UserPackage, SDK::ResourceFileSystem::AppPackage }) {
			SDK::TexturePtr texture{};
			SDK::ResourceLocation location(path, fileSystem);
			dc.renderCtx->getTexture(&texture, location, false);

			if (texture.textureData) {
				dc.drawImage(texture, icon.getPos(), { icon.getWidth(), icon.getHeight() }, d2d::Colors::WHITE);
				logWailaOnce("texture-icon-draw:" + path,
					"Drawing explicit texture '{}' fs={}", path, static_cast<int>(fileSystem));
				return true;
			}
		}

		logWailaOnce("texture-icon-null:" + path,
			"Texture icon '{}' passed PNG proof, but getTexture returned null", path);
		return false;
	}

	ID2D1Bitmap* getD2DTextureBitmap(std::string texturePath, ID2D1DeviceContext* dc) {
		auto path = normalizeTexturePath(std::move(texturePath));
		if (path.empty() || !dc) return nullptr;

		static std::unordered_map<std::string, ComPtr<ID2D1Bitmap>> cache;
		static std::unordered_set<std::string> failed;

		if (auto found = cache.find(path); found != cache.end()) {
			return found->second.Get();
		}
		if (failed.contains(path)) {
			return nullptr;
		}

		std::string buffer;
		if (!loadPngTextureResource(path, &buffer)) {
			failed.emplace(path);
			return nullptr;
		}

		if (buffer.size() > std::numeric_limits<UINT>::max()) {
			logWailaOnce("d2d-texture-too-large:" + path, "PNG '{}' is too large for SHCreateMemStream", path);
			failed.emplace(path);
			return nullptr;
		}

		auto stream = ComPtr<IStream>(SHCreateMemStream(reinterpret_cast<BYTE const*>(buffer.data()),
			static_cast<UINT>(buffer.size())));
		if (!stream) {
			logWailaOnce("d2d-texture-stream:" + path, "Could not create memory stream for PNG '{}'", path);
			failed.emplace(path);
			return nullptr;
		}

		ComPtr<IWICBitmapDecoder> decoder;
		auto* factory = Latite::getRenderer().getImagingFactory();
		if (!factory || FAILED(factory->CreateDecoderFromStream(stream.Get(), nullptr,
			WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf()))) {
			logWailaOnce("d2d-texture-decoder:" + path, "Could not create WIC decoder for PNG '{}'", path);
			failed.emplace(path);
			return nullptr;
		}

		ComPtr<IWICBitmapFrameDecode> frame;
		if (FAILED(decoder->GetFrame(0, frame.GetAddressOf()))) {
			logWailaOnce("d2d-texture-frame:" + path, "Could not decode first frame for PNG '{}'", path);
			failed.emplace(path);
			return nullptr;
		}

		ComPtr<IWICFormatConverter> converter;
		if (FAILED(factory->CreateFormatConverter(converter.GetAddressOf())) ||
			FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
				nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
			logWailaOnce("d2d-texture-converter:" + path, "Could not convert PNG '{}' for D2D", path);
			failed.emplace(path);
			return nullptr;
		}

		ComPtr<ID2D1Bitmap> bitmap;
		if (FAILED(dc->CreateBitmapFromWicBitmap(converter.Get(), nullptr, bitmap.GetAddressOf()))) {
			logWailaOnce("d2d-texture-bitmap:" + path, "Could not create D2D bitmap for PNG '{}'", path);
			failed.emplace(path);
			return nullptr;
		}

		auto [it, _] = cache.emplace(path, std::move(bitmap));
		logWailaOnce("d2d-texture-draw:" + path, "Decoded D2D preview texture '{}'", path);
		return it->second.Get();
	}

	bool drawD2DTextureIcon(DrawUtil& dc, std::string const& texturePath, d2d::Rect const& icon) {
		if (dc.isMinecraft()) return false;

		auto& d2dDc = static_cast<D2DUtil&>(dc);
		auto* bitmap = getD2DTextureBitmap(texturePath, d2dDc.ctx);
		if (!bitmap) return false;

		d2dDc.ctx->DrawBitmap(bitmap, D2D1::RectF(icon.left, icon.top, icon.right, icon.bottom),
			1.f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		return true;
	}

	float getMinecraftFormattedTextWidth(DrawUtil& dc, std::wstring const& text,
	Renderer::FontSelection font, float size, bool cache) {
		float width = 0.f;

		for (auto const& run : util::ParseMinecraftFormatting(text)) {
			if (run.text.empty()) continue;
			width += dc.getTextSize(run.text, font, size, true, cache).x;
		}

		return width;
	}

	void drawMinecraftFormattedText(DrawUtil& dc, d2d::Rect const& rect, std::wstring const& text, d2d::Color const& defaultColor,
		Renderer::FontSelection font, float size, bool cache) {
		float x = rect.left;

		for (auto const& run : util::ParseMinecraftFormatting(text)) {
			if (run.text.empty()) continue;

			float runWidth = dc.getTextSize(run.text, font, size, true, cache).x;

			d2d::Rect runRect = rect;
			runRect.left = x;
			runRect.right = x + runWidth + 4.f;

			auto color = util::MinecraftFormattingColor(run.colorCode, defaultColor);

			dc.drawText(runRect, run.text, color, font, size, DWRITE_TEXT_ALIGNMENT_LEADING,
				DWRITE_PARAGRAPH_ALIGNMENT_NEAR, cache);

			x += runWidth;
		}
	}

	d2d::Color colorForBlockId(std::string const& id) {
		std::string clean = stripIdentifier(id);

		if (clean.find("leaves") != std::string::npos || clean.find("grass") != std::string::npos ||
			clean.find("moss") != std::string::npos || clean.find("vine") != std::string::npos) {
			return d2d::Color::RGB(80, 145, 71);
		}
		if (clean.find("log") != std::string::npos || clean.find("wood") != std::string::npos ||
			clean.find("planks") != std::string::npos) {
			return d2d::Color::RGB(131, 88, 45);
		}
		if (clean.find("water") != std::string::npos || clean.find("ice") != std::string::npos) {
			return d2d::Color::RGB(63, 110, 202);
		}
		if (clean.find("lava") != std::string::npos || clean.find("fire") != std::string::npos) {
			return d2d::Color::RGB(222, 92, 30);
		}
		if (clean.find("sand") != std::string::npos || clean.find("sponge") != std::string::npos) {
			return d2d::Color::RGB(204, 184, 112);
		}
		if (clean.find("ore") != std::string::npos || clean.find("stone") != std::string::npos ||
			clean.find("deepslate") != std::string::npos || clean.find("cobble") != std::string::npos) {
			return d2d::Color::RGB(111, 111, 116);
		}
		if (clean.find("glass") != std::string::npos) {
			return d2d::Color::RGB(126, 185, 200);
		}
		return d2d::Color::RGB(116, 104, 94);
	}

	d2d::Color colorForEntityId(uint32_t id) {
		switch (id) {
		case 319: return d2d::Color::RGB(70, 126, 215);
		case 16777999:
		case 16778099:
		case 886:
			return d2d::Color::RGB(139, 91, 66);
		case 199456:
		case 199468:
		case 199471:
		case 199534:
		case 199540:
			return d2d::Color::RGB(73, 130, 74);
		case 1116962:
		case 1116974:
		case 1116976:
			return d2d::Color::RGB(207, 207, 207);
		case 2849: return d2d::Color::RGB(67, 160, 69);
		case 2854: return d2d::Color::RGB(39, 39, 45);
		case 4880:
		case 4875:
		case 4876:
		case 4877:
			return d2d::Color::RGB(196, 142, 128);
		case 64:
		case 69:
			return d2d::Color::RGB(210, 195, 74);
		default:
			return d2d::Color::RGB(126, 114, 166);
		}
	}

	std::wstring entityNameFromType(uint32_t id) {
		static const std::unordered_map<uint32_t, std::wstring> names = {
			{ 64, L"Item" },
			{ 65, L"Primed TNT" },
			{ 66, L"Falling Block" },
			{ 69, L"Experience Orb" },
			{ 70, L"Eye of Ender" },
			{ 71, L"End Crystal" },
			{ 72, L"Firework Rocket" },
			{ 77, L"Fishing Hook" },
			{ 83, L"Painting" },
			{ 90, L"Boat" },
			{ 93, L"Lightning Bolt" },
			{ 95, L"Area Effect Cloud" },
			{ 117, L"Shield" },
			{ 119, L"Lectern" },
			{ 145, L"Ominous Item Spawner" },
			{ 218, L"Chest Boat" },
			{ 307, L"NPC" },
			{ 312, L"Agent" },
			{ 317, L"Armor Stand" },
			{ 318, L"Tripod Camera" },
			{ 319, L"Player" },
			{ 378, L"Bee" },
			{ 379, L"Piglin" },
			{ 383, L"Piglin Brute" },
			{ 390, L"Allay" },
			{ 788, L"Iron Golem" },
			{ 789, L"Snow Golem" },
			{ 886, L"Wandering Trader" },
			{ 916, L"Copper Golem" },
			{ 2849, L"Creeper" },
			{ 2853, L"Slime" },
			{ 2854, L"Enderman" },
			{ 2857, L"Ghast" },
			{ 2858, L"Magma Cube" },
			{ 2859, L"Blaze" },
			{ 2861, L"Witch" },
			{ 2865, L"Guardian" },
			{ 2866, L"Elder Guardian" },
			{ 2869, L"Ender Dragon" },
			{ 2870, L"Shulker" },
			{ 2873, L"Vindicator" },
			{ 2875, L"Ravager" },
			{ 2920, L"Evoker" },
			{ 2921, L"Vex" },
			{ 2930, L"Pillager" },
			{ 2936, L"Elder Guardian Ghost" },
			{ 2947, L"Warden" },
			{ 2956, L"Breeze" },
			{ 2962, L"Creaking" },
			{ 4874, L"Chicken" },
			{ 4875, L"Cow" },
			{ 4876, L"Pig" },
			{ 4877, L"Sheep" },
			{ 4880, L"Mooshroom" },
			{ 4882, L"Rabbit" },
			{ 4892, L"Polar Bear" },
			{ 4893, L"Llama" },
			{ 4938, L"Turtle" },
			{ 4977, L"Panda" },
			{ 4985, L"Fox" },
			{ 4988, L"Hoglin" },
			{ 4989, L"Strider" },
			{ 4992, L"Goat" },
			{ 4994, L"Axolotl" },
			{ 4996, L"Frog" },
			{ 5002, L"Camel" },
			{ 5003, L"Sniffer" },
			{ 5006, L"Armadillo" },
			{ 5011, L"Happy Ghast" },
			{ 5021, L"Trader Llama" },
			{ 8977, L"Squid" },
			{ 8991, L"Dolphin" },
			{ 9068, L"Pufferfish" },
			{ 9069, L"Salmon" },
			{ 9071, L"Tropical Fish" },
			{ 9072, L"Fish" },
			{ 9089, L"Glow Squid" },
			{ 9093, L"Tadpole" },
			{ 9109, L"Nautilus" },
			{ 21262, L"Wolf" },
			{ 21270, L"Ocelot" },
			{ 21278, L"Parrot" },
			{ 21323, L"Cat" },
			{ 33043, L"Bat" },
			{ 68388, L"Zombified Piglin" },
			{ 68404, L"Wither" },
			{ 68410, L"Phantom" },
			{ 68478, L"Zoglin" },
			{ 68504, L"Camel Husk" },
			{ 76694, L"Zombie Nautilus" },
			{ 199456, L"Zombie" },
			{ 199468, L"Zombie Villager" },
			{ 199471, L"Husk" },
			{ 199534, L"Drowned" },
			{ 199540, L"Zombie Villager" },
			{ 264995, L"Spider" },
			{ 264999, L"Silverfish" },
			{ 265000, L"Cave Spider" },
			{ 265015, L"Endermite" },
			{ 524372, L"Minecart" },
			{ 524384, L"Hopper Minecart" },
			{ 524385, L"TNT Minecart" },
			{ 524386, L"Chest Minecart" },
			{ 524387, L"Furnace Minecart" },
			{ 524388, L"Command Block Minecart" },
			{ 1116962, L"Skeleton" },
			{ 1116974, L"Stray" },
			{ 1116976, L"Wither Skeleton" },
			{ 1117072, L"Bogged" },
			{ 1117079, L"Parched" },
			{ 16777999, L"Villager" },
			{ 16778099, L"Villager" },
			{ 2118423, L"Horse" },
			{ 2118424, L"Donkey" },
			{ 2118425, L"Mule" },
			{ 2186010, L"Skeleton Horse" },
			{ 2186011, L"Zombie Horse" },
			{ 4194372, L"Experience Bottle" },
			{ 4194380, L"Shulker Bullet" },
			{ 4194383, L"Dragon Fireball" },
			{ 4194385, L"Snowball" },
			{ 4194386, L"Egg" },
			{ 4194389, L"Fireball" },
			{ 4194390, L"Potion" },
			{ 4194391, L"Ender Pearl" },
			{ 4194393, L"Wither Skull" },
			{ 4194395, L"Wither Skull" },
			{ 4194398, L"Small Fireball" },
			{ 4194405, L"Lingering Potion" },
			{ 4194406, L"Llama Spit" },
			{ 4194407, L"Evocation Fang" },
			{ 4194410, L"Ice Bomb" },
			{ 4194445, L"Breeze Wind Charge" },
			{ 4194447, L"Wind Charge" },
			{ 12582985, L"Trident" },
			{ 12582992, L"Arrow" },
		};

		if (auto found = names.find(id); found != names.end()) {
			return found->second;
		}

		std::wstringstream ss;
		ss << L"Entity " << id;
		return ss.str();
	}

	std::optional<float> rayIntersectsAABB(Vec3 const& origin, Vec3 const& direction, AABB box, float maxDistance) {
		constexpr float expand = 0.08f;
		box.lower.x -= expand;
		box.lower.y -= expand;
		box.lower.z -= expand;
		box.higher.x += expand;
		box.higher.y += expand;
		box.higher.z += expand;

		float tMin = 0.f;
		float tMax = maxDistance;

		auto testAxis = [&](float originAxis, float directionAxis, float minAxis, float maxAxis) -> bool {
			if (std::fabs(directionAxis) < 0.0001f) {
				return originAxis >= minAxis && originAxis <= maxAxis;
			}

			float inv = 1.f / directionAxis;
			float t1 = (minAxis - originAxis) * inv;
			float t2 = (maxAxis - originAxis) * inv;
			if (t1 > t2) std::swap(t1, t2);

			tMin = std::max(tMin, t1);
			tMax = std::min(tMax, t2);
			return tMin <= tMax;
		};

		if (!testAxis(origin.x, direction.x, box.lower.x, box.higher.x)) return std::nullopt;
		if (!testAxis(origin.y, direction.y, box.lower.y, box.higher.y)) return std::nullopt;
		if (!testAxis(origin.z, direction.z, box.lower.z, box.higher.z)) return std::nullopt;
		if (tMax < 0.f || tMin > maxDistance) return std::nullopt;
		return std::max(0.f, tMin);
	}

	Vec3 rayDirectionFromHit(SDK::HitResult* hit) {
		if (!hit) return {};

		Vec3 toHit = hit->hitPos - hit->start;
		if (hit->hitType != SDK::HitType::AIR && vecLength(toHit) > 0.001f) {
			return normalize(toHit);
		}

		return normalize(hit->end);
	}
}

WAILA::WAILA() : HUDModule("WAILA", L"WAILA", L"Shows the block or entity you are looking at.", HUD) {
	addSetting("showBlocks", L"Blocks", L"Show block information.", showBlocks);
	addSetting("showEntities", L"Entities", L"Show entity information.", showEntities);
	addSetting("showNamespace", L"Namespace", L"Show the source namespace or mod id.", showNamespace);
	addSetting("showCoordinates", L"Coordinates", L"Show target block coordinates.", showCoordinates, "showBlocks"_istrue);
	addSetting("showDistance", L"Distance", L"Show distance to the target.", showDistance);
	addSetting("showHealth", L"Health", L"Show health pips for living entities.", showHealth, "showEntities"_istrue);
	addSliderSetting("entityDistance", L"Entity Distance", L"Maximum entity inspection distance.", entityDistance,
		FloatValue(2.f), FloatValue(12.f), FloatValue(0.5f), "showEntities"_istrue);
	addSliderSetting("textSize", L"Text Size", L"Text size for the inspector.", textSize,
		FloatValue(12.f), FloatValue(30.f), FloatValue(1.f));
	addSetting("background", L"Background", L"Inspector background color.", backgroundColor);
	addSetting("border", L"Border", L"Inspector border color.", borderColor);
	addSetting("titleColor", L"Title", L"Inspector title color.", titleColor);
	addSetting("detailColor", L"Detail", L"Inspector detail color.", detailColor);
	addSliderSetting("radius", L"Radius", L"Corner radius.", radius, FloatValue(0.f), FloatValue(8.f), FloatValue(1.f));

	rect = { 0.f, 0.f, defaultWidth, defaultHeight };
}

void WAILA::afterLoadConfig() {
	bool hasStoredPosition = false;
	if (auto stored = std::get_if<Vec2Value>(&storedPos)) {
		hasStoredPosition = stored->x != 0.f || stored->y != 0.f;
	}

	HUDModule::afterLoadConfig();

	if (hasStoredPosition) return;

	auto ci = SDK::ClientInstance::get();
	if (!ci || !ci->getGuiData()) return;

	auto screenSize = ci->getGuiData()->screenSize;
	setPos({ std::max(0.f, (screenSize.x - defaultWidth) * 0.5f), 8.f });
	storePos(screenSize);
}

void WAILA::render(DrawUtil& dc, bool isDefault, bool inEditor) {
	auto target = getTargetInfo(isDefault || inEditor);
	if (!target.has_value()) return;

	float titleSize = std::get<FloatValue>(textSize).value;
	float detailSize = titleSize * 0.78f;
	float healthHeight = target->type == TargetType::Entity && target->health >= 0.f && std::get<BoolValue>(showHealth).value
		? 9.f
		: 0.f;

	auto plainTitle = util::StripMinecraftFormatting(target->title);
	auto plainDetail = util::StripMinecraftFormatting(target->detail);
	bool cacheText = !isDefault && !inEditor;

	auto titleTextSize = dc.getTextSize(plainTitle, Renderer::FontSelection::SecondaryLight, titleSize, true, cacheText);
	auto detailTextSize = dc.getTextSize(plainDetail, Renderer::FontSelection::SecondaryLight, detailSize, true, cacheText);

	float titleWidth = getMinecraftFormattedTextWidth(dc, target->title, Renderer::FontSelection::SecondaryLight, titleSize, cacheText);
	float detailWidth = getMinecraftFormattedTextWidth(dc, target->detail, Renderer::FontSelection::SecondaryLight, detailSize, cacheText);
	float textWidth = std::max(titleWidth, detailWidth);
	float contentWidth = iconSize + textGap + textWidth;
	float width = std::max(defaultWidth, contentWidth + (paddingX * 2.f));
	float textHeight = titleTextSize.y + detailTextSize.y + healthHeight;
	float height = std::max(defaultHeight, std::max(iconSize, textHeight) + (paddingY * 2.f));

	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	d2d::Rect bounds{ 0.f, 0.f, width, height };
	float cornerRadius = std::get<FloatValue>(radius).value;
	auto background = d2d::Color(std::get<ColorValue>(backgroundColor).getMainColor());
	auto border = d2d::Color(std::get<ColorValue>(borderColor).getMainColor());
	auto title = d2d::Color(std::get<ColorValue>(titleColor).getMainColor());
	auto detail = d2d::Color(std::get<ColorValue>(detailColor).getMainColor());

	dc.fillRoundedRectangle(bounds, background, cornerRadius);
	dc.drawRoundedRectangle(bounds, border, cornerRadius, 1.25f, DrawUtil::OutlinePosition::Inside);
	dc.fillRectangle({ 0.f, 0.f, width, 2.f }, border);

	d2d::Rect icon{ paddingX, paddingY + ((height - (paddingY * 2.f) - iconSize) * 0.5f),
		paddingX + iconSize, paddingY + ((height - (paddingY * 2.f) - iconSize) * 0.5f) + iconSize };
	drawTargetIcon(dc, *target, icon);

	float textLeft = paddingX + iconSize + textGap;
	float y = paddingY;
	drawMinecraftFormattedText(dc, {textLeft, y, width - paddingX, y + titleTextSize.y + 2.f}, target->title, title,
	                           Renderer::FontSelection::SecondaryLight, titleSize, cacheText);
	y += titleTextSize.y;

	if (healthHeight > 0.f) {
		drawHealthPips(dc, textLeft, y + 1.f, target->health);
		y += healthHeight;
	}

	if (!target->detail.empty()) {
		drawMinecraftFormattedText(dc, {textLeft, y, width - paddingX, height - paddingY}, target->detail, detail,
		                           Renderer::FontSelection::SecondaryLight, detailSize, cacheText);
	}

	dc.flush();
}

std::optional<WAILA::TargetInfo> WAILA::getTargetInfo(bool preview) {
	if (preview) {
		return TargetInfo{
			.type = TargetType::Block,
			.title = L"Oak Log",
			.detail = std::get<BoolValue>(showNamespace).value ? L"minecraft" : L"",
			.swatch = d2d::Color::RGB(131, 88, 45),
			.texturePath = "textures/blocks/log_oak",
			.textureUv = previewOakLogUv(),
			.health = -1.f,
			.distance = 0.f,
		};
	}

	auto ci = SDK::ClientInstance::get();
	if (!ci || !ci->minecraft) return std::nullopt;

	auto level = ci->minecraft->getLevel();
	if (!level) return std::nullopt;

	auto hit = level->getHitResult();
	if (!hit) return std::nullopt;

	if (std::get<BoolValue>(showEntities).value) {
		float entityTargetDistance = 0.f;
		if (auto actor = getEntityTarget(hit, entityTargetDistance)) {
			if (auto info = getEntityInfo(actor, entityTargetDistance)) {
				return info;
			}
		}
	}

	if (std::get<BoolValue>(showBlocks).value) {
		return getBlockTarget(hit);
	}

	return std::nullopt;
}

std::optional<WAILA::TargetInfo> WAILA::getBlockTarget(SDK::HitResult* hit) {
	if (!hit || hit->hitType != SDK::HitType::BLOCK) return std::nullopt;

	auto ci = SDK::ClientInstance::get();
	if (!ci) return std::nullopt;

	auto region = ci->getRegion();
	if (!region) return std::nullopt;

	auto block = region->getBlock(hit->hitBlock);
	if (!block || !block->legacyBlock) return std::nullopt;

	auto legacy = block->legacyBlock;
	auto namespacedId = legacy->namespacedId.getString();
	std::string nameSource = legacy->name.getString();
	if (nameSource.empty()) {
		nameSource = "block";
	}

	std::wstring detail;
	if (std::get<BoolValue>(showNamespace).value) {
		detail = util::StrToWStr(namespacedId);
	}
	if (std::get<BoolValue>(showCoordinates).value) {
		detail = appendDetail(std::move(detail), formatBlockPos(hit->hitBlock));
	}
	if (std::get<BoolValue>(showDistance).value) {
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(1) << distance(hit->start, hit->hitPos) << L"m";
		detail = appendDetail(std::move(detail), ss.str());
	}

	return TargetInfo{
		.type = TargetType::Block,
		.title = titleCaseIdentifier(nameSource),
		.detail = detail,
		.swatch = colorForBlockId(nameSource),
		.textureUv = resolveBlockTextureUv(*block, hit->hitBlock, namespacedId.empty() ? nameSource : namespacedId),
		.health = -1.f,
		.distance = distance(hit->start, hit->hitPos),
	};
}

SDK::Actor* WAILA::getEntityTarget(SDK::HitResult* hit, float& distanceOut) {
	auto ci = SDK::ClientInstance::get();
	if (!ci || !ci->minecraft || !hit) return nullptr;

	auto level = ci->minecraft->getLevel();
	auto localPlayer = ci->getLocalPlayer();
	if (!level || !localPlayer) return nullptr;

	float maxDistance = std::max(0.f, std::get<FloatValue>(entityDistance).value);
	float nearestDistance = maxDistance;
	if (hit->hitType == SDK::HitType::BLOCK) {
		float blockDistance = distance(hit->start, hit->hitPos);
		if (blockDistance > 0.001f) {
			nearestDistance = std::min(nearestDistance, blockDistance + 0.05f);
		}
	}

	Vec3 direction = rayDirectionFromHit(hit);
	if (vecLength(direction) <= 0.0001f) return nullptr;

	SDK::Actor* nearestActor = nullptr;
	for (auto actor : level->getRuntimeActorList()) {
		if (!actor || actor == localPlayer || !actor->aabbShape) continue;
		if (actor->isInvisible()) continue;

		auto typeComponent = actor->tryGetComponent<SDK::ActorTypeComponent>();
		if (!typeComponent) continue;

		auto hitDistance = rayIntersectsAABB(hit->start, direction, actor->getBoundingBox(), nearestDistance);
		if (!hitDistance.has_value()) continue;
		if (*hitDistance < nearestDistance) {
			nearestDistance = *hitDistance;
			nearestActor = actor;
		}
	}

	if (nearestActor) {
		distanceOut = nearestDistance;
	}
	return nearestActor;
}

std::optional<WAILA::TargetInfo> WAILA::getEntityInfo(SDK::Actor* actor, float distanceToActor) {
	if (!actor) return std::nullopt;

	auto typeComponent = actor->tryGetComponent<SDK::ActorTypeComponent>();
	if (!typeComponent) return std::nullopt;

	uint32_t type = typeComponent->type;
	std::wstring title = entityNameFromType(type);
	SDK::ItemStack* itemStack = nullptr;

	if (type == 319) {
		auto player = static_cast<SDK::Player*>(actor);
		if (!player->playerName.empty()) {
			title = widen(player->playerName);
		}
	}
	else if (type == 64) {
		auto itemActor = static_cast<SDK::ItemActor*>(actor);
		itemStack = itemActor->getItemStack();
		if (itemStack && itemStack->getItem()) {
			auto hoverName = itemStack->getHoverName();
			if (!hoverName.empty()) {
				title = widen(hoverName);
			}
		}
	}

	std::wstring detail;
	if (std::get<BoolValue>(showNamespace).value) {
		detail = L"minecraft";
	}
	if (std::get<BoolValue>(showDistance).value) {
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(1) << distanceToActor << L"m";
		detail = appendDetail(std::move(detail), ss.str());
	}

	float health = -1.f;
	if (std::get<BoolValue>(showHealth).value && actor->getAttributesComponent()) {
		health = std::clamp(actor->getHealth(), 0.f, 20.f);
	}

	return TargetInfo{
		.type = TargetType::Entity,
		.title = title,
		.detail = detail,
		.swatch = colorForEntityId(type),
		.itemStack = itemStack,
		.health = health,
		.distance = distanceToActor,
	};
}

void WAILA::drawHealthPips(DrawUtil& dc, float x, float y, float health) {
	int filledPips = std::clamp(static_cast<int>(std::ceil(health / 2.f)), 0, 10);
	constexpr float pipSize = 6.f;
	constexpr float gap = 2.f;

	for (int i = 0; i < 10; ++i) {
		d2d::Rect pip{
			x + (i * (pipSize + gap)),
			y,
			x + (i * (pipSize + gap)) + pipSize,
			y + pipSize,
		};

		dc.fillRoundedRectangle(pip, i < filledPips
			? d2d::Color::RGB(226, 55, 65)
			: d2d::Color::RGB(74, 40, 45, 150), 1.5f);
	}
}

void WAILA::drawTargetIcon(DrawUtil& dc, TargetInfo const& target, d2d::Rect const& icon) {
	bool drewIcon = false;

	dc.fillRoundedRectangle(icon, d2d::Color::RGB(20, 20, 24, 150), 2.f);

	if (dc.isMinecraft()) {
		auto& mcDc = static_cast<MCDrawUtil&>(dc);

		if (target.itemStack && target.itemStack->item) {
			mcDc.drawItem(target.itemStack, icon.getPos(), icon.getWidth() / 48.f, 1.f);
			drewIcon = true;

			if (target.itemStack->itemCount > 1) {
				mcDc.drawText(icon, std::to_wstring(target.itemStack->itemCount), d2d::Colors::WHITE,
					Renderer::FontSelection::PrimaryRegular, 11.f, DWRITE_TEXT_ALIGNMENT_TRAILING,
					DWRITE_PARAGRAPH_ALIGNMENT_FAR);
			}
		}
		else {
			drewIcon = drawTextureUvIcon(mcDc, target.textureUv, icon);
			if (!drewIcon) {
				drewIcon = drawTextureIcon(mcDc, target.texturePath, icon);
			}
		}
	}
	else {
		drewIcon = drawD2DTextureIcon(dc, target.texturePath, icon);
	}

	if (!drewIcon) {
		dc.fillRoundedRectangle(icon, target.swatch, 2.f);
	}

	dc.drawRoundedRectangle(icon, d2d::Colors::BLACK.asAlpha(0.45f), 2.f, 1.f, DrawUtil::OutlinePosition::Inside);
}

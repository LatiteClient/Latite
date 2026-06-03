#include "pch.h"
#include "WAILA.h"

#include "mc/common/entity/component/ActorTypeComponent.h"
#include "mc/common/world/actor/item/ItemActor.h"
#include "mc/common/world/level/BlockSource.h"
#include "mc/common/world/level/HitResult.h"
#include "mc/common/world/level/block/Block.h"
#include "client/misc/PlayerHeadCache.h"

namespace {
	constexpr float defaultWidth = 296.f;
	constexpr float defaultHeight = 96.f;
	constexpr float iconSize = 64.f;
	constexpr float paddingX = 8.f;
	constexpr float paddingY = 6.f;
	constexpr float textGap = 8.f;
	constexpr float borderThickness = 3.f;
	constexpr Vec2 skinFaceUvPos{ 0.125f, 0.125f };
	constexpr Vec2 skinFaceUvSize{ 0.125f, 0.125f };

	std::wstring titleCaseIdentifier(std::string id) {
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

	Vec3 rayDirectionFromHit(SDK::HitResult* hit) {
		if (!hit) return {};

		Vec3 toHit = hit->hitPos - hit->start;
		if (hit->hitType != SDK::HitType::AIR && toHit.magnitude() > 0.001f) {
			return toHit.normalized();
		}

		return hit->end.normalized();
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
		FloatValue(20.f), FloatValue(44.f), FloatValue(1.f));
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

	auto clientInstance = SDK::ClientInstance::get();
	if (!clientInstance || !clientInstance->getGuiData()) return;

	Vec2 screenSize = clientInstance->getGuiData()->screenSize;
	setPos({ std::max(0.f, (screenSize.x - defaultWidth) * 0.5f), 8.f });
	storePos(screenSize);
}

void WAILA::render(DrawUtil& dc, bool isDefault, bool inEditor) {
	std::optional<TargetInfo> target = getTargetInfo(isDefault || inEditor);
	if (!target.has_value()) return;

	float titleSize = std::get<FloatValue>(textSize).value;
	float detailSize = titleSize * 0.78f;
	float healthHeight = target->type == TargetType::Entity && target->health >= 0.f && std::get<BoolValue>(showHealth).value
		? 9.f
		: 0.f;

	std::wstring plainTitle = util::StripMinecraftFormatting(target->title);
	std::wstring plainDetail = util::StripMinecraftFormatting(target->detail);
	bool cacheText = !isDefault && !inEditor;

	Vec2 titleTextSize = dc.getTextSize(plainTitle, Renderer::FontSelection::PrimaryRegular, titleSize, true, cacheText);
	Vec2 detailTextSize = dc.getTextSize(plainDetail, Renderer::FontSelection::PrimaryRegular, detailSize, true, cacheText);
	float textWidth = std::max(titleTextSize.x, detailTextSize.x);
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
	dc.drawRoundedRectangle(bounds, border, cornerRadius, borderThickness);

	d2d::Rect icon{ paddingX, paddingY + ((height - (paddingY * 2.f) - iconSize) * 0.5f),
		paddingX + iconSize, paddingY + ((height - (paddingY * 2.f) - iconSize) * 0.5f) + iconSize };
	drawTargetIcon(dc, *target, icon);

	float textLeft = paddingX + iconSize + textGap;
	float y = paddingY;
	dc.drawText({textLeft, y - 1.f, width - paddingX, y + titleTextSize.y + 2.f}, target->title, title,
		Renderer::FontSelection::SecondaryLight, titleSize, DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT_NEAR, cacheText);
	y += titleTextSize.y;

	if (healthHeight > 0.f) {
		drawHealthPips(dc, textLeft, y + 1.f, target->health);
		y += healthHeight;
	}

	if (!target->detail.empty()) {
		dc.drawText({textLeft, y + 1.f, width - paddingX, height - paddingY}, target->detail, detail,
			Renderer::FontSelection::SecondaryLight, detailSize, DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT_NEAR, cacheText);
	}

	dc.flush();
}

std::optional<WAILA::TargetInfo> WAILA::getTargetInfo(bool preview) {
	if (preview) {
		return TargetInfo{
			.type = TargetType::Block,
			.title = L"WAILA",
			.detail = std::get<BoolValue>(showNamespace).value ? L"Minecraft" : L"",
			.health = -1.f,
		};
	}

	auto clientInstance = SDK::ClientInstance::get();
	if (!clientInstance || !clientInstance->minecraft) return std::nullopt;

	auto level = clientInstance->minecraft->getLevel();
	if (!level) return std::nullopt;

	auto hit = level->getHitResult();
	if (!hit) return std::nullopt;

	if (std::get<BoolValue>(showEntities).value) {
		float entityTargetDistance = 0.f;
		if (auto actor = getEntityTarget(hit, entityTargetDistance)) {
			if (std::optional<TargetInfo> info = getEntityInfo(actor, entityTargetDistance)) {
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

	auto clientInstance = SDK::ClientInstance::get();
	if (!clientInstance) return std::nullopt;

	auto region = clientInstance->getRegion();
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
		if (!detail.empty()) detail += L"  ";
		std::wstringstream ss;
		ss << "\n" << hit->hitBlock.x << L", " << hit->hitBlock.y << L", " << hit->hitBlock.z;
		detail += ss.str();
	}
	if (std::get<BoolValue>(showDistance).value) {
		if (!detail.empty()) detail += L"  ";
		std::wstringstream ss;
		ss << "\n" << std::fixed << std::setprecision(1) << hit->start.distance(hit->hitPos) << L"m";
		detail += ss.str();
	}

	return TargetInfo{
		.type = TargetType::Block,
		.title = titleCaseIdentifier(nameSource),
		.detail = detail,
		.block = block,
		.health = -1.f,
	};
}

SDK::Actor* WAILA::getEntityTarget(SDK::HitResult* hit, float& distanceOut) {
	auto clientInstance = SDK::ClientInstance::get();
	if (!clientInstance || !clientInstance->minecraft || !hit) return nullptr;

	auto level = clientInstance->minecraft->getLevel();
	auto localPlayer = clientInstance->getLocalPlayer();
	if (!level || !localPlayer) return nullptr;

	float maxDistance = std::max(0.f, std::get<FloatValue>(entityDistance).value);
	float nearestDistance = maxDistance;
	if (hit->hitType == SDK::HitType::BLOCK) {
		float blockDistance = hit->start.distance(hit->hitPos);
		if (blockDistance > 0.001f) {
			nearestDistance = std::min(nearestDistance, blockDistance + 0.05f);
		}
	}

	Vec3 direction = rayDirectionFromHit(hit);
	if (direction.magnitude() <= 0.0001f) return nullptr;

	SDK::Actor* nearestActor = nullptr;
	for (auto actor : level->getRuntimeActorList()) {
		if (!actor || actor == localPlayer || !actor->aabbShape) continue;
		if (actor->isInvisible()) continue;

		auto typeComponent = actor->tryGetComponent<SDK::ActorTypeComponent>();
		if (!typeComponent) continue;

		std::optional<float> hitDistance = actor->getBoundingBox().intersectsRay(hit->start, direction, nearestDistance, 0.08f);
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
	std::string faceTexturePath;
	Vec2 faceUvPos = skinFaceUvPos;
	Vec2 faceUvSize = skinFaceUvSize;

	if (actor->isPlayer()) {
		auto player = static_cast<SDK::Player*>(actor);
		if (!player->playerName.empty()) {
			title = util::StrToWStr(player->playerName);
		}

		faceTexturePath = getPlayerFaceTexturePath(player);
		if (!faceTexturePath.empty()) {
			faceUvPos = { 0.f, 0.f };
			faceUvSize = { 1.f, 1.f };
		}
	}
	else if (actor->isItem()) {
		auto itemActor = static_cast<SDK::ItemActor*>(actor);
		itemStack = itemActor->getItemStack();
		if (itemStack && itemStack->getItem()) {
			auto hoverName = itemStack->getHoverName();
			if (!hoverName.empty()) {
				title = util::StrToWStr(hoverName);
			}
		}
	}

	std::wstring detail;
	if (std::get<BoolValue>(showNamespace).value) {
		detail = L"Minecraft";
	}
	if (std::get<BoolValue>(showDistance).value) {
		if (!detail.empty()) detail += L"  ";
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(1) << distanceToActor << L"m";
		detail += ss.str();
	}

	float health = -1.f;
	if (std::get<BoolValue>(showHealth).value && actor->getAttributesComponent() && !actor->isItem()) {
		health = std::clamp(actor->getHealth(), 0.f, 20.f);
	}

	return TargetInfo{
		.type = TargetType::Entity,
		.title = title,
		.detail = detail,
		.actor = actor,
		.itemStack = itemStack,
		.faceTexturePath = faceTexturePath,
		.faceUvPos = faceUvPos,
		.faceUvSize = faceUvSize,
		.health = health,
	};
}

std::string WAILA::getPlayerFaceTexturePath(SDK::Player* player) {
	if (!player) return {};

	auto clientInstance = SDK::ClientInstance::get();
	if (!clientInstance || !clientInstance->minecraft) return {};

	if (auto playerSkin = player->getSkin()) {
		auto texturePath = PlayerHeadCache::getTexturePath(*playerSkin);
		if (!texturePath.empty()) {
			return texturePath;
		}
	}

	if (player->playerName.empty()) return {};

	auto level = clientInstance->minecraft->getLevel();
	if (!level) return {};

	auto playerList = level->getPlayerList();
	if (!playerList) return {};

	for (auto& entryPair : *playerList) {
		auto& entry = entryPair.second;
		if (entry.name != player->playerName) continue;

		return PlayerHeadCache::getTexturePath(entry.skin);
	}

	return {};
}

// TODO: draw real minecraft hearts?
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

		dc.fillRectangle(pip, i < filledPips
			? d2d::Color::RGB(226, 55, 65)
			: d2d::Color::RGB(74, 40, 45, 150));
	}
}

void WAILA::drawTargetIcon(DrawUtil& ctxGeneric, TargetInfo const& target, d2d::Rect const& icon) {
	// if i dont draw a super small rectangle before drawing items everything gets fucked for some reason
	//ctxGeneric.fillRoundedRectangle(d2d::Rect(icon.left / 48.f, icon.top / 48.f, icon.right / 48.f, icon.bottom / 48.f),
	                                //d2d::Color::RGB(20, 20, 24, 150), 2.f);

	auto& dc = static_cast<MCDrawUtil&>(ctxGeneric);

	if (!target.faceTexturePath.empty()) {
		SDK::TexturePtr texture{};
		auto externalTexture = std::filesystem::path(target.faceTexturePath).is_absolute();
		auto textureFileSystem = externalTexture ? SDK::ResourceFileSystem::Raw : SDK::ResourceFileSystem::UserPackage;
		dc.renderCtx->getTexture(&texture, SDK::ResourceLocation(target.faceTexturePath, textureFileSystem), externalTexture);
		if (texture.textureData) {
			dc.drawImage(texture, icon.getPos(), icon.getSize(), target.faceUvPos, target.faceUvSize, d2d::Colors::WHITE);
			return;
		}
	}

	if (target.itemStack && target.itemStack->getItem()) {
		dc.drawItem(target.itemStack, icon.getPos(), icon.getWidth() / 48.f, 1.f);

		if (target.itemStack->itemCount > 1) {
			dc.drawText(icon, std::to_wstring(target.itemStack->itemCount), d2d::Colors::WHITE,
				Renderer::FontSelection::PrimaryRegular, 11.f, DWRITE_TEXT_ALIGNMENT_TRAILING,
				DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		}
	}
	else if (target.type == TargetType::Block && target.block &&
		Signatures::ItemStack_ItemStackBlock.result && Signatures::ItemStackBase_destructor.result) {
		alignas(SDK::ItemStack) char storage[sizeof(SDK::ItemStack)] = {};
		SDK::ItemStack* itemStack = SDK::ItemStack::constructFromBlock(storage, *target.block, 1, nullptr);
		if (itemStack) {
			itemStack->showPickUp = false;
			itemStack->wasPickedUp = false;
			itemStack->pickupTime = {};

			if (itemStack->getItem()) {
				dc.drawItem(itemStack, icon.getPos(), icon.getWidth() / 48.f, 1.f);
			}

			itemStack->destruct();
		}
	}
	else if (target.type == TargetType::Entity && target.actor && !target.actor->isItem()) {
		dc.drawActor(target.actor, icon, 1.f);
	}

}

#include "pch.h"
#include "WAILA.h"

#include "mc/common/entity/component/ActorTypeComponent.h"
#include "mc/common/locale/I18n.h"
#include "mc/common/world/DiggerItem.h"
#include "mc/common/world/WeaponItem.h"
#include "mc/common/world/actor/item/ItemActor.h"
#include "mc/common/world/level/BlockSource.h"
#include "mc/common/world/level/HitResult.h"
#include "mc/common/world/level/block/Block.h"
#include "client/misc/PlayerHeadCache.h"

WAILA::WAILA() : HUDModule("WAILA", L"WAILA", L"Shows the block or entity you are looking at.", HUD) {
	showPreview = false;

	addSetting("showBlocks", L"Blocks", L"Show block information.", showBlocks);
	addSetting("showEntities", L"Entities", L"Show entity information.", showEntities);
	addSetting("showNamespace", L"Namespace", L"Show the source namespace or mod id.", showNamespace);
	addSetting("showItemId", L"Item ID", L"Show the full namespaced block or item id.", showItemId,
	           "showNamespace"_istrue);
	addSetting("showCoordinates", L"Coordinates", L"Show target block coordinates.", showCoordinates,
	           "showBlocks"_istrue);
	addSetting("showDistance", L"Distance", L"Show distance to the target.", showDistance);
	addSetting("showHarvest", L"Tool Info", L"Show the preferred tool for the targeted block.",
	           showHarvest, "showBlocks"_istrue);
	toolMode.addEntry(EnumEntry {
		static_cast<int>(ToolMode::HighestTier), L"Highest Tier",
		L"Show the highest-tier effective tool for breaking a block."
	});
	toolMode.addEntry(EnumEntry {
		static_cast<int>(ToolMode::MinimumTier), L"Minimum Tier",
		L"Show the lowest-tier effective tool that can correctly harvest the block."
	});
	toolMode.addEntry(EnumEntry {
		static_cast<int>(ToolMode::Fastest), L"Fastest",
		L"Show the tool with the highest native destroy speed."
	});
	addEnumSetting("toolMode", L"Tool Mode", L"How the preferred mining tool is selected.", toolMode,
	               "showHarvest"_istrue);
	addSetting("showHealth", L"Health", L"Show health pips for living entities.", showHealth, "showEntities"_istrue);
	addSliderSetting("entityDistance", L"Entity Distance", L"Maximum entity inspection distance.", entityDistance,
	                 FloatValue(2.f), FloatValue(12.f), FloatValue(0.5f), "showEntities"_istrue);
	addSliderSetting("textSize", L"Text Size", L"Text size for the inspector.", textSize,
	                 FloatValue(20.f), FloatValue(44.f), FloatValue(1.f));
	addSetting("titleColor", L"Title", L"Inspector title color.", titleColor);
	addSetting("detailColor", L"Detail", L"Inspector detail color.", detailColor);

	rect = { 0.f, 0.f, panelLayout.defaultWidth, panelLayout.defaultHeight };
}

SDK::ItemTier const *WAILA::getToolTier(SDK::Item *item, std::string_view itemId) const {
	if (itemId.ends_with("_sword") || itemId == "minecraft:mace") {
		return static_cast<SDK::WeaponItem *>(item)->tier;
	}
	if (itemId.ends_with("_pickaxe") || itemId.ends_with("_axe") ||
	    itemId.ends_with("_shovel") || itemId.ends_with("_hoe")) {
		return static_cast<SDK::DiggerItem *>(item)->tier;
	}
	return nullptr;
}

std::vector<std::string> WAILA::findPreferredToolItemIds(SDK::Block const &block, ToolMode mode) {
	auto &cache = mode == ToolMode::MinimumTier ? minimumTierToolCache :
	              mode == ToolMode::HighestTier ? highestTierToolCache : fastestToolCache;
	if (auto cached = cache.find(&block); cached != cache.end()) return cached->second;

	if (!Signatures::ItemStack_ItemStackBlock.result || !Signatures::ItemStackBase_destructor.result) return {};

	auto clientInstance = SDK::ClientInstance::get();
	auto level = clientInstance && clientInstance->minecraft ? clientInstance->minecraft->getLevel() : nullptr;
	
	void *registry = nullptr;
	if (level) {
		registry = *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(level) + 0x198);
	}
	if (!registry) return {};

	auto itemCounters = reinterpret_cast<void ***>(reinterpret_cast<uintptr_t>(registry) + 0x30);
	if (!itemCounters[0] || !itemCounters[1]) return {};

	alignas(SDK::ItemStack) char storage[sizeof(SDK::ItemStack)] = {};
	auto candidateStack = SDK::ItemStack::constructFromBlock(storage, block, 1, nullptr);
	if (!candidateStack) return {};

	auto originalItem = candidateStack->item;
	auto originalBlock = candidateStack->block;
	auto originalAux = candidateStack->aux;
	float bestDestroySpeed = 1.f;
	std::vector<std::string> bestItemIds;
	struct ToolFamilyCandidate {
		void **vtable;
		SDK::ItemTier const *tier;
		float destroySpeed;
		std::string itemId;
	};
	std::vector<ToolFamilyCandidate> tierCandidates;

	auto shouldReplaceCandidate = [&](SDK::ItemTier const *tier, float destroySpeed, ToolFamilyCandidate const &family) {
		if (tier && !family.tier) return true;
		if (!tier && family.tier) return false;

		bool sameTier = (!tier && !family.tier) || (tier->level == family.tier->level && std::abs(tier->speed - family.tier->speed) <= toolLayout.speedEpsilon);

		if (!sameTier) {
			if (mode == ToolMode::MinimumTier) {
				return tier->level < family.tier->level || (tier->level == family.tier->level && tier->speed < family.tier->speed - toolLayout.speedEpsilon);
			} else {
				return tier->level > family.tier->level || (tier->level == family.tier->level && tier->speed > family.tier->speed + toolLayout.speedEpsilon);
			}
		}

		if (mode == ToolMode::MinimumTier) {
			return destroySpeed < family.destroySpeed - toolLayout.speedEpsilon;
		} else {
			return destroySpeed > family.destroySpeed + toolLayout.speedEpsilon;
		}
	};

	for (auto current = itemCounters[0]; current < itemCounters[1]; ++current) {
		auto counter = *current;
		auto item = counter ? *reinterpret_cast<SDK::Item **>(counter) : nullptr;
		if (!item) continue;

		candidateStack->item = reinterpret_cast<SDK::Item **>(counter);
		candidateStack->block = nullptr;
		candidateStack->aux = 0;
		candidateStack->itemCount = 1;

		float destroySpeed = item->getDestroySpeed(candidateStack, &block);
		if (destroySpeed <= 1.f) continue;

		std::string itemId = item->namespacedId.getString();
		if (itemId.empty()) continue;

		if (mode == ToolMode::MinimumTier || mode == ToolMode::HighestTier) {
			if (!item->canDestroySpecial(&block)) continue;

			auto vtable = *reinterpret_cast<void ***>(item);
			auto tier = getToolTier(item, itemId);
			auto family = std::ranges::find(tierCandidates, vtable, &ToolFamilyCandidate::vtable);
			if (family == tierCandidates.end()) {
				tierCandidates.push_back({ vtable, tier, destroySpeed, std::move(itemId) });
			} else if (shouldReplaceCandidate(tier, destroySpeed, *family)) {
				family->tier = tier;
				family->destroySpeed = destroySpeed;
				family->itemId = std::move(itemId);
			}
			continue;
		}

		if (destroySpeed > bestDestroySpeed + toolLayout.speedEpsilon) {
			bestDestroySpeed = destroySpeed;
			bestItemIds.clear();
			bestItemIds.push_back(std::move(itemId));
		} else if (std::abs(destroySpeed - bestDestroySpeed) <= toolLayout.speedEpsilon &&
		           std::ranges::find(bestItemIds, itemId) == bestItemIds.end()) {
			bestItemIds.push_back(std::move(itemId));
		}
	}

	if (mode == ToolMode::MinimumTier || mode == ToolMode::HighestTier) {
		bestItemIds.reserve(tierCandidates.size());
		for (auto &candidate: tierCandidates) {
			bestItemIds.push_back(std::move(candidate.itemId));
		}
	}

	candidateStack->item = originalItem;
	candidateStack->block = originalBlock;
	candidateStack->aux = originalAux;
	candidateStack->destruct();
	cache.insert_or_assign(&block, bestItemIds);
	return bestItemIds;
}

std::optional<std::wstring> WAILA::getLocalizedMinecraftName(std::string const &key) const {
	if (key.empty()) return std::nullopt;

	auto i18n = SDK::I18n::get();
	if (!i18n) return std::nullopt;

	auto localizedName = i18n->get(key);
	if (localizedName.empty() || localizedName == key || localizedName == "%" + key) {
		return std::nullopt;
	}
	return util::StrToWStr(localizedName);
}

std::wstring WAILA::getBlockDisplayName(
	SDK::Block const &block, std::string const &localizationKey, std::string const &fallbackName) const {
	if (Signatures::ItemStackBase_getHoverName.result &&
	    Signatures::ItemStack_ItemStackBlock.result &&
	    Signatures::ItemStackBase_destructor.result) {
		alignas(SDK::ItemStack) char storage[sizeof(SDK::ItemStack)] = {};
		auto itemStack = SDK::ItemStack::constructFromBlock(storage, block, 1, nullptr);
		if (itemStack) {
			auto hoverName = itemStack->getHoverName();
			itemStack->destruct();
			if (!hoverName.empty()) {
				return util::StrToWStr(hoverName);
			}
		}
	}

	if (!localizationKey.empty()) {
		auto key = localizationKey.ends_with(".name") ? localizationKey : localizationKey + ".name";
		if (auto localizedName = getLocalizedMinecraftName(key)) {
			return *localizedName;
		}
	}

	return titleCaseIdentifier(fallbackName);
}

void WAILA::drawInspectorPanel(DrawUtil &dc, d2d::Rect const &bounds) const {
	auto &mc = static_cast<MCDrawUtil &>(dc);
	if (!mc.renderCtx) return;

	const auto background = d2d::Color::RGB(0x12, 0x0B, 0x16, 218);
	const d2d::Color borderTones[] = {
		d2d::Color::RGB(0x59, 0x20, 0x83, 238),
		d2d::Color::RGB(0x56, 0x23, 0x7F, 238),
		d2d::Color::RGB(0x52, 0x25, 0x7B, 238),
		d2d::Color::RGB(0x4E, 0x26, 0x76, 238),
		d2d::Color::RGB(0x4A, 0x25, 0x71, 238),
		d2d::Color::RGB(0x46, 0x23, 0x6C, 238),
		d2d::Color::RGB(0x42, 0x20, 0x67, 238),
		d2d::Color::RGB(0x3E, 0x1D, 0x62, 238),
	};

	float left = bounds.left;
	float top = bounds.top;
	float right = bounds.right;
	float bottom = bounds.bottom;
	if (right - left <= panelLayout.frameThickness * 4.f ||
	    bottom - top <= panelLayout.frameThickness * 4.f)
		return;

	// Deferred UI meshes must be resolved before the item renderer changes render state.
	mc.flush(false);
	mc.setImmediate(true);

	// Leave a single frame unit open at each outer corner.
	mc.fillRectangle({ left + panelLayout.frameThickness, top, right - panelLayout.frameThickness, bottom },
	                 background);
	mc.fillRectangle({
		                 left, top + panelLayout.frameThickness, left + panelLayout.frameThickness,
		                 bottom - panelLayout.frameThickness
	                 }, background);
	mc.fillRectangle({
		                 right - panelLayout.frameThickness, top + panelLayout.frameThickness, right,
		                 bottom - panelLayout.frameThickness
	                 }, background);

	float borderLeft = left + panelLayout.frameThickness;
	float borderTopY = top + panelLayout.frameThickness;
	float borderRight = right - panelLayout.frameThickness;
	float borderBottomY = bottom - panelLayout.frameThickness;
	float sideTop = borderTopY + panelLayout.frameThickness;
	float sideBottom = borderBottomY - panelLayout.frameThickness;
	float toneHeight = (sideBottom - sideTop) / static_cast<float>(std::size(borderTones));

	mc.fillRectangle({ borderLeft, borderTopY, borderRight, borderTopY + panelLayout.frameThickness },
	                 borderTones[0]);
	for (size_t i = 0; i < std::size(borderTones); ++i) {
		float toneTop = sideTop + (toneHeight * static_cast<float>(i));
		float toneBottom = i + 1 == std::size(borderTones)
			                   ? sideBottom
			                   : sideTop + (toneHeight * static_cast<float>(i + 1));
		mc.fillRectangle({ borderLeft, toneTop, borderLeft + panelLayout.frameThickness, toneBottom },
		                 borderTones[i]);
		mc.fillRectangle({ borderRight - panelLayout.frameThickness, toneTop, borderRight, toneBottom },
		                 borderTones[i]);
	}
	mc.fillRectangle({ borderLeft, borderBottomY - panelLayout.frameThickness, borderRight, borderBottomY },
	                 borderTones[std::size(borderTones) - 1]);

	mc.setImmediate(false);
}

std::wstring WAILA::titleCaseIdentifier(std::string id) const {
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
	for (char ch: id) {
		if (ch == '_' || ch == '-' || ch == '.') {
			out.push_back(L' ');
			newWord = true;
			continue;
		}

		unsigned char uch = static_cast<unsigned char>(ch);
		if (newWord) {
			out.push_back(static_cast<wchar_t>(std::toupper(uch)));
		} else {
			out.push_back(static_cast<wchar_t>(std::tolower(uch)));
		}
		newWord = false;
	}

	if (out == L"Tnt") return L"TNT";
	if (out.empty()) return L"Unknown";
	return out;
}

Vec3 WAILA::rayDirectionFromHit(SDK::HitResult *hit) const {
	if (!hit) return {};

	Vec3 toHit = hit->hitPos - hit->start;
	if (hit->hitType != SDK::HitType::AIR && toHit.magnitude() > 0.001f) {
		return toHit.normalized();
	}

	return hit->end.normalized();
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
	setPos({ std::max(0.f, (screenSize.x - panelLayout.defaultWidth) * 0.5f), 8.f });
	storePos(screenSize);
}

void WAILA::render(DrawUtil &dc, bool isDefault, bool inEditor) {
	if (!dc.isMinecraft()) return;

	std::optional<TargetInfo> target = getTargetInfo(isDefault || inEditor);
	if (!target.has_value()) return;

	float titleSize = std::get<FloatValue>(textSize).value * panelLayout.titleTextScale;
	float detailSize = titleSize * panelLayout.detailTextScale;
	bool hasDetail = !target->detail.empty();
	bool showTargetHealth = target->type == TargetType::Entity && target->health >= 0.f &&
	                        std::get<BoolValue>(showHealth).value;
	int targetMaxHearts = target->maxHealth > 0.f
		                      ? static_cast<int>(std::ceil(target->maxHealth / 2.f))
		                      : healthLayout.heartsPerRow;
	int heartRows = showTargetHealth
		                ? (targetMaxHearts + healthLayout.heartsPerRow - 1) / healthLayout.heartsPerRow
		                : 0;
	float healthHeight = heartRows > 0 ? healthLayout.heartSize * heartRows : 0.f;

	std::wstring plainTitle = util::StripMinecraftFormatting(target->title);
	std::wstring plainDetail = util::StripMinecraftFormatting(target->detail);
	bool cacheText = !isDefault && !inEditor;

	Vec2 titleTextSize = dc.getTextSize(plainTitle, Renderer::FontSelection::PrimaryRegular, titleSize, true,
	                                    cacheText);
	Vec2 detailTextSize = dc.getTextSize(plainDetail, Renderer::FontSelection::PrimaryRegular, detailSize, true,
	                                     cacheText);
	if (!hasDetail) {
		detailTextSize = {};
	}

	float textWidth = std::max(titleTextSize.x, detailTextSize.x);
	bool showToolIcons = target->type == TargetType::Block && !target->toolItemIds.empty();
	float toolIconsWidth = showToolIcons
		                       ? (toolLayout.iconSize * static_cast<float>(target->toolItemIds.size())) +
		                         (toolLayout.spacing * static_cast<float>(target->toolItemIds.size() - 1))
		                       : 0.f;
	if (healthHeight > 0.f) {
		int heartsInWidestRow = std::min(targetMaxHearts, healthLayout.heartsPerRow);
		float healthWidth = healthLayout.heartSize * heartsInWidestRow;
		textWidth = std::max(textWidth, healthWidth);
	}
	float contentWidth = panelLayout.iconSlotSize + panelLayout.textGap + textWidth +
	                     (showToolIcons ? toolLayout.gap + toolIconsWidth : 0.f);
	float width = std::max(panelLayout.defaultWidth, contentWidth + (panelLayout.paddingX * 2.f));
	float textHeight = titleTextSize.y;
	if (healthHeight > 0.f) textHeight += panelLayout.lineGap + healthHeight;
	if (hasDetail) textHeight += panelLayout.lineGap + detailTextSize.y;
	float height = std::max(panelLayout.defaultHeight,
	                        std::max(panelLayout.iconSlotSize, textHeight) + (panelLayout.paddingY * 2.f));

	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	d2d::Rect bounds { 0.f, 0.f, width, height };
	auto title = d2d::Color(std::get<ColorValue>(titleColor).getMainColor());
	auto detail = d2d::Color(std::get<ColorValue>(detailColor).getMainColor());

	drawInspectorPanel(dc, bounds);

	float iconSlotTop = panelLayout.paddingY +
	                    ((height - (panelLayout.paddingY * 2.f) - panelLayout.iconSlotSize) * 0.5f);
	d2d::Rect icon {
		panelLayout.paddingX + panelLayout.iconInset, iconSlotTop + panelLayout.iconInset,
		panelLayout.paddingX + panelLayout.iconInset + panelLayout.iconSize,
		iconSlotTop + panelLayout.iconInset + panelLayout.iconSize
	};
	drawTargetIcon(dc, *target, icon);

	float textLeft = panelLayout.paddingX + panelLayout.iconSlotSize + panelLayout.textGap;
	float textRight = width - panelLayout.paddingX - (showToolIcons ? toolLayout.gap + toolIconsWidth : 0.f);
	float y = panelLayout.paddingY +
	          std::max(0.f, (height - (panelLayout.paddingY * 2.f) - textHeight) * 0.5f);
	dc.drawText({ textLeft, y, textRight, y + titleTextSize.y + 1.f }, target->title, title,
	            Renderer::FontSelection::SecondaryLight, titleSize, DWRITE_TEXT_ALIGNMENT_LEADING,
	            DWRITE_PARAGRAPH_ALIGNMENT_NEAR, cacheText);
	y += titleTextSize.y;

	if (healthHeight > 0.f) {
		y += panelLayout.lineGap;
		drawHealthHearts(dc, textLeft - 1.5f, y, target->health, target->maxHealth);
		y += healthHeight;
	}

	if (hasDetail) {
		y += panelLayout.lineGap;
		dc.drawText({ textLeft, y, textRight, height - panelLayout.paddingY }, target->detail, detail,
		            Renderer::FontSelection::SecondaryLight, detailSize, DWRITE_TEXT_ALIGNMENT_LEADING,
		            DWRITE_PARAGRAPH_ALIGNMENT_NEAR, cacheText);
	}

	if (showToolIcons && dc.isMinecraft() && Signatures::ItemStack_ItemStackBlock.result &&
	    Signatures::ItemStackBase_destructor.result) {
		auto clientInstance = SDK::ClientInstance::get();
		auto level = clientInstance && clientInstance->minecraft ? clientInstance->minecraft->getLevel() : nullptr;
		
		void *registry = nullptr;
		if (level) {
			registry = *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(level) + 0x198);
		}
		std::vector<void *> toolCounters(target->toolItemIds.size());
		bool hasToolCounter = false;
		if (registry) {
			auto itemCounters = reinterpret_cast<void ***>(reinterpret_cast<uintptr_t>(registry) + 0x30);
			for (auto current = itemCounters[0]; current && current < itemCounters[1]; ++current) {
				auto counter = *current;
				auto item = counter ? *reinterpret_cast<SDK::Item **>(counter) : nullptr;
				if (!item) continue;

				auto itemId = item->namespacedId.getString();
				auto targetItem = std::ranges::find(target->toolItemIds, itemId);
				if (targetItem != target->toolItemIds.end()) {
					toolCounters[std::distance(target->toolItemIds.begin(), targetItem)] = counter;
					hasToolCounter = true;
				}
			}
		}

		if (target->block && hasToolCounter) {
			alignas(SDK::ItemStack) char storage[sizeof(SDK::ItemStack)] = {};
			auto toolStack = SDK::ItemStack::constructFromBlock(storage, *target->block, 1, nullptr);
			if (toolStack) {
				auto originalItem = toolStack->item;
				auto originalBlock = toolStack->block;
				auto originalAux = toolStack->aux;

				auto &mc = static_cast<MCDrawUtil &>(dc);
				mc.flush(false);
				float toolX = width - panelLayout.paddingX - toolIconsWidth;
				for (auto toolCounter: toolCounters) {
					if (toolCounter) {
						toolStack->item = reinterpret_cast<SDK::Item **>(toolCounter);
						toolStack->block = nullptr;
						toolStack->aux = 0;
						toolStack->itemCount = 1;
						mc.drawItem(toolStack, { toolX, height - panelLayout.paddingY - toolLayout.iconSize },
						            toolLayout.iconSize / 48.f, 1.f);
					}
					toolX += toolLayout.iconSize + toolLayout.spacing;
				}

				toolStack->item = originalItem;
				toolStack->block = originalBlock;
				toolStack->aux = originalAux;
				toolStack->destruct();
			}
		}
	}

	dc.flush();
}

std::optional<WAILA::TargetInfo> WAILA::getTargetInfo(bool preview) {
	if (preview) {
		bool namespaceEnabled = std::get<BoolValue>(showNamespace).value;
		bool itemIdEnabled = std::get<BoolValue>(showItemId).value;
		return TargetInfo {
			.type = TargetType::Block,
			.title = L"Stone",
			.detail = namespaceEnabled
				          ? (itemIdEnabled ? L"\u00A7ominecraft:stone\u00A7r" : L"\u00A7oMinecraft\u00A7r") // italicize namespace
				          : L"",
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

std::optional<WAILA::TargetInfo> WAILA::getBlockTarget(SDK::HitResult *hit) {
	if (!hit || hit->hitType != SDK::HitType::BLOCK) return std::nullopt;

	auto clientInstance = SDK::ClientInstance::get();
	if (!clientInstance) return std::nullopt;

	auto region = clientInstance->getRegion();
	if (!region) return std::nullopt;

	auto block = region->getBlock(hit->hitBlock);
	if (!block || !block->legacyBlock) return std::nullopt;

	auto legacy = block->legacyBlock;
	auto namespacedId = legacy->namespacedId.getString();
	auto localizationKey = legacy->translateName;
	std::string nameSource = legacy->name.getString();
	if (nameSource.empty()) {
		nameSource = "block";
	}

	std::wstring detail;
	if (std::get<BoolValue>(showNamespace).value) {
		if (std::get<BoolValue>(showItemId).value) {
			// italicize namespace
			detail = L"\u00A7o" + util::StrToWStr(namespacedId) + L"\u00A7r";
		} else {
			auto separator = namespacedId.find(':');
			auto namespaceId = separator == std::string::npos ? namespacedId : namespacedId.substr(0, separator);
			// italicize namespace
			detail = L"\u00A7o" + titleCaseIdentifier(namespaceId) + L"\u00A7r";
		}
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
	if (std::get<BoolValue>(showHarvest).value) {
		return TargetInfo {
			.type = TargetType::Block,
			.title = getBlockDisplayName(*block, localizationKey, nameSource),
			.detail = detail,
			.block = block,
			.toolItemIds = findPreferredToolItemIds(
				*block, static_cast<ToolMode>(toolMode.getSelectedKey())),
			.health = -1.f,
		};
	}

	return TargetInfo {
		.type = TargetType::Block,
		.title = getBlockDisplayName(*block, localizationKey, nameSource),
		.detail = detail,
		.block = block,
		.health = -1.f,
	};
}

SDK::Actor *WAILA::getEntityTarget(SDK::HitResult *hit, float &distanceOut) {
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

	SDK::Actor *nearestActor = nullptr;
	for (auto actor: level->getRuntimeActorList()) {
		if (!actor || actor == localPlayer || !actor->aabbShape) continue;
		if (actor->isInvisible()) continue;

		auto typeComponent = actor->tryGetComponent<SDK::ActorTypeComponent>();
		if (!typeComponent) continue;

		std::optional<float> hitDistance = actor->getBoundingBox().intersectsRay(
			hit->start, direction, nearestDistance, 0.08f);
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

std::optional<WAILA::TargetInfo> WAILA::getEntityInfo(SDK::Actor *actor, float distanceToActor) {
	if (!actor) return std::nullopt;

	auto typeComponent = actor->tryGetComponent<SDK::ActorTypeComponent>();
	if (!typeComponent) return std::nullopt;

	TargetInfo info {
		.type = TargetType::Entity,
		.actor = actor,
	};

	auto localizationKey = actor->getEntityLocalizationKey();
	if (auto localizedName = getLocalizedMinecraftName(localizationKey)) {
		info.title = *localizedName;
	} else {
		auto typeName = actor->getEntityTypeName();
		info.title = titleCaseIdentifier(typeName.empty() ? "entity" : typeName);
	}

	if (actor->isPlayer()) {
		auto player = static_cast<SDK::Player *>(actor);
		if (!player->playerName.empty()) {
			info.title = util::StrToWStr(player->playerName);
		}

		info.faceTexturePath = getPlayerFaceTexturePath(player);
		if (!info.faceTexturePath.empty()) {
			info.faceUvPos = { 0.f, 0.f };
			info.faceUvSize = { 1.f, 1.f };
		}
	} else if (actor->isItem()) {
		auto itemActor = static_cast<SDK::ItemActor *>(actor);
		info.itemStack = itemActor->getItemStack();
		if (info.itemStack && info.itemStack->getItem()) {
			auto hoverName = info.itemStack->getHoverName();
			if (!hoverName.empty()) {
				info.title = util::StrToWStr(hoverName);
			}
		}
	}

	if (std::get<BoolValue>(showNamespace).value) {
		auto entityNamespace = actor->getEntityNamespace();
		if (!entityNamespace.empty()) {
			// italicize namespace
			info.detail = L"\u00A7o" + titleCaseIdentifier(entityNamespace) + L"\u00A7r";
		}
	}
	if (std::get<BoolValue>(showDistance).value) {
		if (!info.detail.empty()) info.detail += L"  ";
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(1) << distanceToActor << L"m";
		info.detail += ss.str();
	}

	if (std::get<BoolValue>(showHealth).value && actor->getHealth() != std::nullopt && !actor->isItem()) {
		info.health = actor->getHealth().value();
		if (actor->getMaxHealth() != std::nullopt) {
			info.maxHealth = actor->getMaxHealth().value();
		} else {
			info.maxHealth = 20.0f;
		}
	}

	return info;
}

std::string WAILA::getPlayerFaceTexturePath(SDK::Player *player) const {
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

	for (auto &entryPair: *playerList) {
		auto &entry = entryPair.second;
		if (entry.name != player->playerName) continue;

		return PlayerHeadCache::getTexturePath(entry.skin);
	}

	return {};
}

Vec2 WAILA::getHeartPosition(float x, float y, int index) const {
	return {
		x + ((index % healthLayout.heartsPerRow) * healthLayout.heartSize),
		y + ((index / healthLayout.heartsPerRow) * healthLayout.heartSize),
	};
}

void WAILA::drawHealthHearts(DrawUtil &dc, float x, float y, float health, float maxHealth) const {
	int maxHearts = maxHealth > 0
		                ? static_cast<int>(std::ceil(maxHealth / 2.f))
		                : healthLayout.heartsPerRow;
	int halfHearts = std::clamp(static_cast<int>(std::ceil(health)), 0, maxHearts * 2);
	int filledHearts = halfHearts / 2;
	bool hasHalfHeart = (halfHearts % 2) != 0;

	auto &mc = static_cast<MCDrawUtil &>(dc);
	if (mc.renderCtx) {
		SDK::TexturePtr backgroundTexture {};
		SDK::TexturePtr heartTexture {};
		SDK::TexturePtr halfHeartTexture {};
		mc.renderCtx->getTexture(&backgroundTexture,
		                         SDK::ResourceLocation("/resource_packs/vanilla/textures/ui/heart_background",
		                                               SDK::ResourceFileSystem::AppPackage), false);
		mc.renderCtx->getTexture(&heartTexture,
		                         SDK::ResourceLocation("/resource_packs/vanilla/textures/ui/heart",
		                                               SDK::ResourceFileSystem::AppPackage),
		                         false);
		mc.renderCtx->getTexture(&halfHeartTexture,
		                         SDK::ResourceLocation("/resource_packs/vanilla/textures/ui/heart_half",
		                                               SDK::ResourceFileSystem::AppPackage),
		                         false);

		if (backgroundTexture.textureData && heartTexture.textureData && halfHeartTexture.textureData) {
			for (int i = 0; i < maxHearts; ++i) {
				Vec2 position = getHeartPosition(x, y, i);
				mc.renderCtx->drawImage(backgroundTexture,
				                        { position.x * mc.guiScale, position.y * mc.guiScale },
				                        { healthLayout.heartSize * mc.guiScale, healthLayout.heartSize * mc.guiScale },
				                        { 0.f, 0.f },
				                        { 1.f, 1.f });
			}
			mc.renderCtx->flushImages(d2d::Colors::WHITE, 1.f, SDK::HashedString("ui_textured_and_glcolor_sprite"));

			if (filledHearts > 0) {
				for (int i = 0; i < filledHearts; ++i) {
					Vec2 position = getHeartPosition(x, y, i);
					mc.renderCtx->drawImage(heartTexture,
					                        { position.x * mc.guiScale, position.y * mc.guiScale },
					                        {
						                        healthLayout.heartSize * mc.guiScale,
						                        healthLayout.heartSize * mc.guiScale
					                        },
					                        { 0.f, 0.f },
					                        { 1.f, 1.f });
				}
				mc.renderCtx->flushImages(d2d::Colors::WHITE, 1.f, SDK::HashedString("ui_textured_and_glcolor_sprite"));
			}

			if (hasHalfHeart && filledHearts < maxHearts) {
				Vec2 position = getHeartPosition(x, y, filledHearts);
				mc.renderCtx->drawImage(halfHeartTexture,
				                        { position.x * mc.guiScale, position.y * mc.guiScale },
				                        { healthLayout.heartSize * mc.guiScale, healthLayout.heartSize * mc.guiScale },
				                        { 0.f, 0.f },
				                        { 1.f, 1.f });
				mc.renderCtx->flushImages(d2d::Colors::WHITE, 1.f, SDK::HashedString("ui_textured_and_glcolor_sprite"));
			}

			return;
		}
	}

	for (int i = 0; i < maxHearts; ++i) {
		Vec2 position = getHeartPosition(x, y, i);
		d2d::Rect heart {
			position.x,
			position.y,
			position.x + healthLayout.heartSize,
			position.y + healthLayout.heartSize,
		};

		dc.fillRectangle(heart, d2d::Color::RGB(74, 40, 45, 150));

		if (i < filledHearts) {
			dc.fillRectangle(heart, d2d::Color::RGB(226, 55, 65));
		} else if (i == filledHearts && hasHalfHeart) {
			d2d::Rect halfHeart = heart;
			halfHeart.right = halfHeart.left + (heart.getWidth() * 0.5f);
			dc.fillRectangle(halfHeart, d2d::Color::RGB(226, 55, 65));
		}
	}
}

void WAILA::drawTargetIcon(DrawUtil &ctxGeneric, TargetInfo const &target, d2d::Rect const &icon) const {
	auto &dc = static_cast<MCDrawUtil &>(ctxGeneric);

	if (target.type == TargetType::Entity && target.actor && !target.actor->isItem()) {
		float actorIconExpansion = (icon.getWidth() * (panelLayout.actorIconScale - 1.f)) * 0.5f;
		d2d::Rect actorIcon {
			icon.left - actorIconExpansion,
			icon.top - actorIconExpansion,
			icon.right + actorIconExpansion,
			icon.bottom + actorIconExpansion,
		};
		if (dc.drawActor(target.actor, actorIcon, 1.f)) {
			return;
		}
	}

	if (!target.faceTexturePath.empty()) {
		SDK::TexturePtr texture {};
		auto externalTexture = std::filesystem::path(target.faceTexturePath).is_absolute();
		auto textureFileSystem = externalTexture ? SDK::ResourceFileSystem::Raw : SDK::ResourceFileSystem::UserPackage;
		dc.renderCtx->getTexture(&texture, SDK::ResourceLocation(target.faceTexturePath, textureFileSystem),
		                         externalTexture);
		if (texture.textureData) {
			dc.drawImage(texture, icon.getPos(), icon.getSize(), target.faceUvPos, target.faceUvSize,
			             d2d::Colors::WHITE);
			return;
		}
	}

	if (target.itemStack && target.itemStack->getItem()) {
		dc.flush(false);
		dc.drawItem(target.itemStack, icon.getPos(), icon.getWidth() / 48.f, 1.f);

		if (target.itemStack->itemCount > 1) {
			dc.drawText(icon, std::to_wstring(target.itemStack->itemCount), d2d::Colors::WHITE,
			            Renderer::FontSelection::PrimaryRegular, 21.f, DWRITE_TEXT_ALIGNMENT_TRAILING,
			            DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		}
	} else if (target.type == TargetType::Block && target.block &&
	           Signatures::ItemStack_ItemStackBlock.result && Signatures::ItemStackBase_destructor.result) {
		alignas(SDK::ItemStack) char storage[sizeof(SDK::ItemStack)] = {};
		SDK::ItemStack *itemStack = SDK::ItemStack::constructFromBlock(storage, *target.block, 1, nullptr);
		if (itemStack) {
			itemStack->showPickUp = false;
			itemStack->wasPickedUp = false;
			itemStack->pickupTime = {};

			if (itemStack->getItem()) {
				dc.flush(false);
				dc.drawItem(itemStack, icon.getPos(), icon.getWidth() / 48.f, 1.f);
			}

			itemStack->destruct();
		}
	}
}

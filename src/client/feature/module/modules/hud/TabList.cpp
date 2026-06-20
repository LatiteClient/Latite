#include "pch.h"
#include "TabList.h"
#include "client/Latite.h"
#include "client/event/events/RenderNameTagEvent.h"
#include "client/event/events/RenderLayerEvent.h"
#include "client/event/events/TickEvent.h"
#include "client/misc/PlayerHeadCache.h"
#include "client/misc/NameTagCache.h"
#include "mc/common/client/gui/controls/UIControl.h"
#include "mc/common/client/gui/controls/VisualTree.h"
#include "mc/common/world/actor/player/SerializedSkinRef.h"
#include "mc/common/world/level/Level.h"
#include "util/Logger.h"
#include "util/DrawContext.h"
#include <algorithm>
#include <unordered_set>
#include <vector>

TabList::TabList() : Module("PlayerList", LocalizeString::get("client.module.tabList.name"),
                            LocalizeString::get("client.module.tabList.desc"), HUD, VK_TAB) {
    addSetting("textColor", LocalizeString::get("client.module.tabList.textColor.name"),
               LocalizeString::get("client.module.tabList.textColor.desc"), textCol);
    addSetting("bgColor", LocalizeString::get("client.module.tabList.bgColor.name"),
               LocalizeString::get("client.module.tabList.bgColor.desc"), bgCol);
	addSliderSetting("textSize", LocalizeString::get("client.textmodule.props.textSize.name"), L"", textSizeS,
		FloatValue(2.f), FloatValue(100.f), FloatValue(2.f));
    listen<RenderLayerEvent>(static_cast<EventListenerFunc>(&TabList::onRenderLayer));
	listen<RenderNameTagEvent>(static_cast<EventListenerFunc>(&TabList::onRenderNameTag));
	listen<TickEvent>(static_cast<EventListenerFunc>(&TabList::onTick));
}

bool TabList::skinKeyMatches(PlayerHeadSkinKey const& key, SDK::SerializedSkinRef const& skin, SDK::SkinImage const& image) const {
	auto id = skin.getId();

	if (id && key.id != *id) return false;
	if (!id && !key.id.empty()) return false;

	if (key.width != image.width) return false;
	if (key.height != image.height) return false;

	if (key.bytes != reinterpret_cast<uintptr_t>(image.bytes.data())) return false;
	if (key.byteCount != image.bytes.size()) return false;

	return true;
}

TabList::PlayerHeadSkinKey TabList::makeSkinKey(SDK::SerializedSkinRef const& skin, SDK::SkinImage const& image) const {
	PlayerHeadSkinKey key {};
	if (auto id = skin.getId(); id) {
		key.id = *id;
	}
	key.width = image.width;
	key.height = image.height;
	key.bytes = reinterpret_cast<uintptr_t>(image.bytes.data());
	key.byteCount = image.bytes.size();
	return key;
}

std::string TabList::getRowName(SDK::PlayerListEntry& entry) const {
	if (auto formattedName = Latite::get().getNameTagCache().getFormattedPlayerName(entry.name)) return *formattedName;
	return entry.name;
}

std::unordered_set<std::string> TabList::getActivePlayerNames(SDK::Level* level) const {
	std::unordered_set<std::string> players;
	if (!level || !level->getPlayerList()) return players;

	players.reserve(level->getPlayerList()->size());
	for (auto& ent : *level->getPlayerList()) {
		players.insert(ent.second.name);
	}
	return players;
}

std::vector<SDK::PlayerListEntry*> TabList::getSortedPlayerListRows(SDK::Level* level) const {
	std::vector<SDK::PlayerListEntry*> rows;
	if (!level || !level->getPlayerList()) return rows;

	rows.reserve(level->getPlayerList()->size());
	for (auto& ent : *level->getPlayerList()) {
		rows.push_back(&ent.second);
	}

	std::stable_sort(rows.begin(), rows.end(), [this](auto* a, auto* b) {
		std::string aName = getRowName(*a);
		std::string bName = getRowName(*b);
		auto& nameTags = Latite::get().getNameTagCache();
		const int aColor = nameTags.getFirstColorSortIndex(aName);
		const int bColor = nameTags.getFirstColorSortIndex(bName);
		if (aColor != bColor) return aColor < bColor;
		return nameTags.stripFormatCodes(aName) < nameTags.stripFormatCodes(bName);
	});

	return rows;
}

void TabList::drawPlayerHead(MCDrawUtil& dc, SDK::PlayerListEntry& entry, d2d::Rect const& bounds) const {
	if (!dc.renderCtx) return;

	auto image = entry.skin.getSkinImage();
	if (!image) return;

	auto& cachedHead = cachedHeadTextures[entry.name];
	if (!cachedHead.hasSkinKey || !skinKeyMatches(cachedHead.skinKey, entry.skin, *image)) {
		cachedHead = {};
		cachedHead.skinKey = makeSkinKey(entry.skin, *image);
		cachedHead.hasSkinKey = true;
		cachedHead.texturePath = PlayerHeadCache::getTexturePath(entry.skin);
	}

	if (cachedHead.texturePath.empty()) return;

	if (!cachedHead.texture.textureData) {
		dc.renderCtx->getTexture(&cachedHead.texture,
			SDK::ResourceLocation(cachedHead.texturePath, SDK::ResourceFileSystem::Raw), true);
		if (!cachedHead.texture.textureData) return;
	}

	dc.drawImage(cachedHead.texture, bounds.getPos(), bounds.getSize(), d2d::Colors::WHITE);
}

ColorValue TabList::getColorOrDefault(ValueType const& value, ColorValue const& fallback) const {
	if (!std::holds_alternative<ColorValue>(value)) return fallback;
	return std::get<ColorValue>(value);
}

float TabList::getFloatOrDefault(ValueType const& value, float fallback) const {
	if (!std::holds_alternative<FloatValue>(value)) return fallback;
	return std::get<FloatValue>(value).value;
}

void TabList::onRenderNameTag(Event& evG) {
	RenderNameTagEvent& ev = static_cast<RenderNameTagEvent&>(evG);
	std::string* tag = ev.getNametag();
	if (!tag || !Latite::get().getNameTagCache().hasFormatCode(*tag)) return;

	SDK::ClientInstance* clientInstance = SDK::ClientInstance::get();
	if (!clientInstance || !clientInstance->minecraft) return;

	SDK::Level* level = clientInstance->minecraft->getLevel();
	if (!level || !level->getPlayerList()) return;

	Latite::get().getNameTagCache().recordRenderedNameTag(*tag, getActivePlayerNames(level));
}

void TabList::onTick(Event& evG) {
	auto& ev = static_cast<TickEvent&>(evG);
	auto* level = ev.getLevel();
	if (!level || !level->getPlayerList()) {
		Latite::get().getNameTagCache().updateFormattedPlayerNames({});
		return;
	}

	Latite::get().getNameTagCache().updateFormattedPlayerNames(getActivePlayerNames(level));
}

void TabList::afterLoadConfig() {
	if (!std::holds_alternative<ColorValue>(textCol)) {
		Logger::Warn("TabList: textColor setting was invalid, restoring default");
		textCol = ColorValue(1.f, 1.f, 1.f, 1.f);
	}
	if (!std::holds_alternative<ColorValue>(bgCol)) {
		Logger::Warn("TabList: bgColor setting was invalid, restoring default");
		bgCol = ColorValue(0.f, 0.f, 0.f, 0.5f);
	}
	if (!std::holds_alternative<FloatValue>(textSizeS)) {
		Logger::Warn("TabList: textSize setting was invalid, restoring default");
		textSizeS = FloatValue(20.f);
	}
}

void TabList::onRenderLayer(Event& evG) {
	auto& ev = static_cast<RenderLayerEvent&>(evG);
	auto* screenView = ev.getScreenView();
	if (!screenView || !screenView->visualTree || !screenView->visualTree->rootControl
		|| screenView->visualTree->rootControl->name != "hud_screen") return;

	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;

	MCDrawUtil dc{ ev.getUIRenderContext(), Latite::get().getFont() };
	dc.setImmediate(false);
	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
	if (!lvl || !lvl->getPlayerList()) return;

	size_t size = lvl->getPlayerList()->size();

	float textP = getFloatOrDefault(textSizeS, 20.f);

	std::wstring txt;
	if (SDK::RakNetConnector::get() && SDK::RakNetConnector::get()->featuredServer.size() > 0) {
		txt = util::StrToWStr(SDK::RakNetConnector::get()->featuredServer);
	}
	else {
		txt = util::StrToWStr(lvl->getLevelName());
	}

	constexpr auto font = Renderer::FontSelection::PrimaryRegular;
	const ColorValue textColor = getColorOrDefault(textCol, ColorValue(1.f, 1.f, 1.f, 1.f));
	const ColorValue backgroundColor = getColorOrDefault(bgCol, ColorValue(0.f, 0.f, 0.f, 0.5f));
	float sectionHeight = textP * 1.3f;
	float headSize = std::max(1.f, std::min(textP, sectionHeight - 4.f));
	float headGap = std::max(2.f, textP * 0.2f);
	float headInset = 2.f;
	float rowTextOffset = headInset + headSize + headGap;

	float longestText = dc.getTextSize(txt, font, textP).x;
	auto sortedRows = getSortedPlayerListRows(lvl);
	for (auto* row : sortedRows) {
		std::string rowName = getRowName(*row);
		auto w = rowTextOffset + dc.getTextSize(util::StrToWStr(Latite::get().getNameTagCache().stripFormatCodes(rowName)), font, textP).x + 3.f;
		if (w > longestText) longestText = w;
	}

	float sectionSize = longestText;

	size_t maxPerTab = 16;
	int numTabs = static_cast<int>(std::ceil(static_cast<float>(size) / static_cast<float>(maxPerTab)));

	float oY = sectionHeight;

	float x = 0.f;
	float y = oY;

	size_t idx = 0;

	float calcWidth = sectionSize * static_cast<float>(numTabs);
	float calcHeight = sectionHeight * ((static_cast<float>(size) > maxPerTab) ? maxPerTab : static_cast<float>(size));

	auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;
	const Vec2 offset = { ss.x / 2.f - calcWidth / 2.f, 20.f };

	dc.fillRectangle({ offset.x, offset.y, offset.x + calcWidth, offset.y + calcHeight + oY }, backgroundColor.getMainColor());
	dc.drawRectangle({ offset.x, offset.y, offset.x + calcWidth, offset.y + calcHeight + oY }, d2d::Color(backgroundColor.getMainColor()).asAlpha(1.f), 2.f);

	dc.drawText({ offset.x, offset.y, offset.x + calcWidth, offset.y + oY }, txt, textColor.getMainColor(), font, textP, DWRITE_TEXT_ALIGNMENT_CENTER);
	dc.flush(false, true);

	for (auto* row : sortedRows) {
		std::string rowName = getRowName(*row);
		d2d::Rect rc = { offset.x + x, offset.y + y, offset.x + x + longestText, offset.y + y + sectionHeight };
		d2d::Rect headRect = {
			rc.left + headInset,
			rc.top + ((sectionHeight - headSize) * 0.5f),
			rc.left + headInset + headSize,
			rc.top + ((sectionHeight - headSize) * 0.5f) + headSize
		};
		drawPlayerHead(dc, *row, headRect);

		d2d::Rect textRect = rc;
		textRect.left += rowTextOffset;

		dc.drawText(textRect, util::StrToWStr(rowName), textColor.getMainColor(), font, textP,
			DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);

		idx++;
		if (idx < maxPerTab) {
			y += sectionHeight;
			continue;
		}
		x += longestText;
		y = oY;
		idx = 0;
	}

	dc.flush();
}

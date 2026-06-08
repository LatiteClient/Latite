#include "pch.h"
#include "TabList.h"
#include "client/Latite.h"
#include "client/event/events/RenderLayerEvent.h"
#include "client/event/events/TickEvent.h"
#include "client/misc/NameTagCache.h"
#include "mc/common/client/gui/controls/UIControl.h"
#include "mc/common/client/gui/controls/VisualTree.h"
#include "mc/common/world/actor/player/Player.h"
#include "util/Logger.h"
#include "util/DrawContext.h"
#include <algorithm>
#include <cctype>
#include <unordered_set>
#include <vector>

namespace {
	bool readFormatCode(std::string const& text, size_t index, char& code, size_t& codeSize) {
		if (index + 1 < text.size() && static_cast<unsigned char>(text[index]) == 0xA7) {
			code = text[index + 1];
			codeSize = 2;
			return true;
		}
		if (index + 2 < text.size()
			&& static_cast<unsigned char>(text[index]) == 0xC2
			&& static_cast<unsigned char>(text[index + 1]) == 0xA7) {
			code = text[index + 2];
			codeSize = 3;
			return true;
		}
		return false;
	}

	bool isColorCode(char code) {
		return std::isxdigit(static_cast<unsigned char>(code));
	}

	d2d::Color minecraftColor(char code, d2d::Color const& fallback) {
		switch (static_cast<char>(std::tolower(static_cast<unsigned char>(code)))) {
		case '0': return d2d::Color::RGB(0, 0, 0);
		case '1': return d2d::Color::RGB(0, 0, 170);
		case '2': return d2d::Color::RGB(0, 170, 0);
		case '3': return d2d::Color::RGB(0, 170, 170);
		case '4': return d2d::Color::RGB(170, 0, 0);
		case '5': return d2d::Color::RGB(170, 0, 170);
		case '6': return d2d::Color::RGB(255, 170, 0);
		case '7': return d2d::Color::RGB(170, 170, 170);
		case '8': return d2d::Color::RGB(85, 85, 85);
		case '9': return d2d::Color::RGB(85, 85, 255);
		case 'a': return d2d::Color::RGB(85, 255, 85);
		case 'b': return d2d::Color::RGB(85, 255, 255);
		case 'c': return d2d::Color::RGB(255, 85, 85);
		case 'd': return d2d::Color::RGB(255, 85, 255);
		case 'e': return d2d::Color::RGB(255, 255, 85);
		case 'f': return d2d::Color::RGB(255, 255, 255);
		case 'r': return fallback;
		default: return fallback;
		}
	}

	std::string stripFormatCodes(std::string const& text) {
		std::string stripped;
		stripped.reserve(text.size());
		for (size_t i = 0; i < text.size();) {
			char code = 0;
			size_t codeSize = 0;
			if (readFormatCode(text, i, code, codeSize)) {
				i += codeSize;
				continue;
			}
			stripped += text[i++];
		}
		return stripped;
	}

	bool hasFormatCode(std::string const& text) {
		for (size_t i = 0; i < text.size();) {
			char code = 0;
			size_t codeSize = 0;
			if (readFormatCode(text, i, code, codeSize)) return true;
			i++;
		}
		return false;
	}

	int colorCodeIndex(char code) {
		if (code >= '0' && code <= '9') return code - '0';
		if (code >= 'a' && code <= 'f') return code - 'a' + 10;
		if (code >= 'A' && code <= 'F') return code - 'A' + 10;
		return 16;
	}

	int firstColorSortIndex(std::string const& text) {
		for (size_t i = 0; i < text.size();) {
			char code = 0;
			size_t codeSize = 0;
			if (readFormatCode(text, i, code, codeSize)) {
				if (isColorCode(code)) return colorCodeIndex(code);
				i += codeSize;
				continue;
			}
			i++;
		}
		return 16;
	}

	std::string tabRowName(SDK::PlayerListEntry& entry, std::unordered_map<std::string, std::string> const& coloredNameCache) {
		if (auto it = coloredNameCache.find(entry.name); it != coloredNameCache.end()) return it->second;
		return entry.name;
	}

	std::vector<SDK::PlayerListEntry*> sortedPlayerListRows(SDK::Level* level,
		std::unordered_map<std::string, std::string> const& coloredNameCache) {
		std::vector<SDK::PlayerListEntry*> rows;
		if (!level || !level->getPlayerList()) return rows;

		rows.reserve(level->getPlayerList()->size());
		for (auto& ent : *level->getPlayerList()) {
			rows.push_back(&ent.second);
		}

		std::stable_sort(rows.begin(), rows.end(), [&](auto* a, auto* b) {
			std::string aName = tabRowName(*a, coloredNameCache);
			std::string bName = tabRowName(*b, coloredNameCache);
			const int aColor = firstColorSortIndex(aName);
			const int bColor = firstColorSortIndex(bName);
			if (aColor != bColor) return aColor < bColor;
			return stripFormatCodes(aName) < stripFormatCodes(bName);
		});

		return rows;
	}

	std::string colorizedPlayerName(std::string const& playerName, std::string const& nameTag) {
		const size_t namePos = stripFormatCodes(nameTag).find(playerName);
		if (namePos == std::string::npos) return playerName;
		const size_t nameEnd = namePos + playerName.size();

		std::string activeColor;
		std::string formattedName;
		size_t visiblePos = 0;
		for (size_t i = 0; i < nameTag.size();) {
			char code = 0;
			size_t codeSize = 0;
			if (readFormatCode(nameTag, i, code, codeSize)) {
				if (isColorCode(code) || code == 'r' || code == 'R') {
					activeColor.assign(nameTag, i, codeSize);
				}
				if (visiblePos >= namePos && visiblePos < nameEnd) {
					formattedName.append(nameTag, i, codeSize);
				}
				i += codeSize;
				continue;
			}
			if (visiblePos >= nameEnd) break;
			if (visiblePos >= namePos) {
				if (formattedName.empty()) formattedName += activeColor;
				formattedName += nameTag[i];
			}
			visiblePos++;
			i++;
		}
		return formattedName.empty() ? playerName : formattedName;
	}

	void drawFormattedText(DrawUtil& dc, d2d::Rect const& rc, std::string const& text, d2d::Color const& fallbackColor,
		Renderer::FontSelection font, float textSize) {
		float x = rc.left;
		d2d::Color color = fallbackColor;
		std::string segment;
		auto flush = [&]() {
			if (segment.empty()) return;
			std::wstring wide = util::StrToWStr(segment);
			dc.drawText({ x, rc.top, rc.right, rc.bottom }, wide, color, font, textSize,
				DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);
			x += dc.getTextSize(wide, font, textSize, true, false).x;
			segment.clear();
		};

		for (size_t i = 0; i < text.size();) {
			char code = 0;
			size_t codeSize = 0;
			if (readFormatCode(text, i, code, codeSize)) {
				flush();
				if (isColorCode(code) || std::tolower(static_cast<unsigned char>(code)) == 'r') {
					color = minecraftColor(code, fallbackColor).asAlpha(fallbackColor.a);
				}
				i += codeSize;
				continue;
			}
			segment += text[i++];
		}
		flush();
	}

	ColorValue colorOrDefault(ValueType const& value, ColorValue const& fallback) {
		if (!std::holds_alternative<ColorValue>(value)) return fallback;
		return std::get<ColorValue>(value);
	}

	float floatOrDefault(ValueType const& value, float fallback) {
		if (!std::holds_alternative<FloatValue>(value)) return fallback;
		return std::get<FloatValue>(value).value;
	}
}

TabList::TabList() : Module("PlayerList", LocalizeString::get("client.module.tabList.name"),
                            LocalizeString::get("client.module.tabList.desc"), HUD, VK_TAB) {
    addSetting("textColor", LocalizeString::get("client.module.tabList.textColor.name"),
               LocalizeString::get("client.module.tabList.textColor.desc"), textCol);
    addSetting("bgColor", LocalizeString::get("client.module.tabList.bgColor.name"),
               LocalizeString::get("client.module.tabList.bgColor.desc"), bgCol);
	addSliderSetting("textSize", LocalizeString::get("client.textmodule.props.textSize.name"), L"", textSizeS,
		FloatValue(2.f), FloatValue(100.f), FloatValue(2.f));
    listen<RenderLayerEvent>(static_cast<EventListenerFunc>(&TabList::onRenderLayer));
	listen<TickEvent>(static_cast<EventListenerFunc>(&TabList::onTick));
}

void TabList::onTick(Event& evG) {
	auto& ev = static_cast<TickEvent&>(evG);
	auto* level = ev.getLevel();
	if (!level || !level->getPlayerList()) {
		coloredNameCache.clear();
		return;
	}

	std::unordered_set<std::string> activePlayers;
	activePlayers.reserve(level->getPlayerList()->size());
	for (auto& ent : *level->getPlayerList()) {
		activePlayers.insert(ent.second.name);
	}

	std::unordered_set<uint64_t> activeRuntimeIds;
	for (auto* actor : level->getRuntimeActorList()) {
		if (!actor || !actor->isPlayer()) continue;

		auto runtimeId = actor->getRuntimeID();
		activeRuntimeIds.insert(runtimeId);

		auto* player = static_cast<SDK::Player*>(actor);
		if (!activePlayers.contains(player->playerName)) continue;

		auto nameTag = NameTagCache::getNetworkNameTag(runtimeId);
		if (!nameTag) {
			coloredNameCache.erase(player->playerName);
			continue;
		}

		std::string rowName = colorizedPlayerName(player->playerName, *nameTag);
		if (hasFormatCode(rowName)) {
			coloredNameCache[player->playerName] = rowName;
		} else {
			coloredNameCache.erase(player->playerName);
		}
	}
	NameTagCache::retainNetworkNameTags(activeRuntimeIds);

	for (auto it = coloredNameCache.begin(); it != coloredNameCache.end();) {
		if (!activePlayers.contains(it->first)) {
			it = coloredNameCache.erase(it);
			continue;
		}
		++it;
	}
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

	float textP = floatOrDefault(textSizeS, 20.f);

	std::wstring txt;
	if (SDK::RakNetConnector::get() && SDK::RakNetConnector::get()->featuredServer.size() > 0) {
		txt = util::StrToWStr(SDK::RakNetConnector::get()->featuredServer);
	}
	else {
		txt = util::StrToWStr(lvl->getLevelName());
	}

	constexpr auto font = Renderer::FontSelection::PrimaryRegular;
	const ColorValue textColor = colorOrDefault(textCol, ColorValue(1.f, 1.f, 1.f, 1.f));
	const ColorValue backgroundColor = colorOrDefault(bgCol, ColorValue(0.f, 0.f, 0.f, 0.5f));
	float sectionHeight = textP * 1.3f;

	float longestText = dc.getTextSize(txt, font, textP).x;
	auto sortedRows = sortedPlayerListRows(lvl, coloredNameCache);
	for (auto* row : sortedRows) {
		std::string rowName = tabRowName(*row, coloredNameCache);
		auto w = dc.getTextSize(util::StrToWStr(stripFormatCodes(rowName)), font, textP).x + 3.f;
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

	for (auto* row : sortedRows) {
		std::string rowName = tabRowName(*row, coloredNameCache);
		d2d::Rect rc = { offset.x + x, offset.y + y, offset.x + x + longestText, offset.y + y + sectionHeight };

		drawFormattedText(dc, rc, rowName, textColor.getMainColor(), font, textP);

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

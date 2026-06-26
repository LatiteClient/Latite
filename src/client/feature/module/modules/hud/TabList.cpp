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
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

TabList::TabList()
    : Module("PlayerList", LocalizeString::get("client.module.tabList.name"),
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

bool TabList::skinKeyMatches(PlayerHeadSkinKey const& key, SDK::SerializedSkinRef const& skin,
                             SDK::SkinImage const& image) const {
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

bool TabList::refreshActivePlayerNames(SDK::Level* level) {
    std::unordered_set<std::string> players;
    if (level && level->getPlayerList()) {
        players.reserve(level->getPlayerList()->size());
        for (auto& ent : *level->getPlayerList()) {
            players.insert(ent.second.name);
        }
    }

    bool changed = players.size() != cachedActivePlayerNames.size();
    if (!changed) {
        for (auto const& playerName : players) {
            if (!cachedActivePlayerNames.contains(playerName)) {
                changed = true;
                break;
            }
        }
    }

    if (changed) {
        cachedActivePlayerNames = std::move(players);
        rowsDirty = true;
        pruneHeadTextureCache();
    }

    return changed;
}

void TabList::syncNameTagCache(SDK::Level* level) {
    auto& nameTags = Latite::get().getNameTagCache();
    bool playersChanged = refreshActivePlayerNames(level);
    uint64_t networkRevision = nameTags.getNetworkNameTagsRevision();
    bool networkChanged = cachedNetworkNameTagsRevision != networkRevision;

    if (playersChanged || networkChanged) {
        if (networkChanged) {
            rowsDirty = true;
        }
        if (nameTags.updateFormattedPlayerNames(cachedActivePlayerNames)) {
            rowsDirty = true;
        }
        cachedNetworkNameTagsRevision = networkRevision;
    }
}

void TabList::rebuildRows(SDK::Level* level) {
    cachedRows.clear();
    if (!level || !level->getPlayerList()) {
        rowsDirty = false;
        layoutDirty = true;
        return;
    }

    auto& nameTags = Latite::get().getNameTagCache();
    cachedRows.reserve(level->getPlayerList()->size());
    for (auto& ent : *level->getPlayerList()) {
        CachedPlayerRow row {};
        row.playerName = ent.second.name;
        row.displayName = getRowName(ent.second);
        row.strippedName = nameTags.stripFormatCodes(row.displayName);
        row.displayNameWide = util::StrToWStr(row.displayName);
        row.strippedNameWide = util::StrToWStr(row.strippedName);
        row.colorSortIndex = nameTags.getFirstColorSortIndex(row.displayName);
        cachedRows.push_back(std::move(row));
    }

    std::stable_sort(cachedRows.begin(), cachedRows.end(), [](CachedPlayerRow const& a, CachedPlayerRow const& b) {
        if (a.colorSortIndex != b.colorSortIndex) return a.colorSortIndex < b.colorSortIndex;
        return a.strippedName < b.strippedName;
    });

    rowsDirty = false;
    layoutDirty = true;
}

void TabList::updateLayoutMeasurements(MCDrawUtil& dc, std::wstring const& title, float textSize, float rowTextOffset) {
    if (!layoutDirty && cachedLayoutTitle == title && cachedLayoutTextSize == textSize &&
        cachedLayoutRowTextOffset == rowTextOffset) {
        return;
    }

    constexpr auto font = Renderer::FontSelection::PrimaryRegular;

    float longestText = dc.getTextSize(title, font, textSize).x;
    for (auto& row : cachedRows) {
        row.measuredWidth = rowTextOffset + dc.getTextSize(row.strippedNameWide, font, textSize).x + 3.f;
        if (row.measuredWidth > longestText) longestText = row.measuredWidth;
    }

    cachedLayoutTitle = title;
    cachedLayoutTextSize = textSize;
    cachedLayoutRowTextOffset = rowTextOffset;
    cachedLongestText = longestText;
    layoutDirty = false;
}

void TabList::pruneHeadTextureCache() {
    for (auto it = cachedHeadTextures.begin(); it != cachedHeadTextures.end();) {
        if (cachedActivePlayerNames.contains(it->first))
            ++it;
        else
            it = cachedHeadTextures.erase(it);
    }
}

void TabList::drawPlayerHead(MCDrawUtil& dc, SDK::PlayerListEntry& entry, std::string const& cacheKey,
                             d2d::Rect const& bounds) const {
    if (!dc.renderCtx) return;

    auto image = entry.skin.getSkinImage();
    if (!image) return;

    auto now = std::chrono::steady_clock::now();
    auto& cachedHead = cachedHeadTextures[cacheKey];
    if (!cachedHead.hasSkinKey || !skinKeyMatches(cachedHead.skinKey, entry.skin, *image)) {
        cachedHead = {};
        cachedHead.skinKey = makeSkinKey(entry.skin, *image);
        cachedHead.hasSkinKey = true;
    }

    if (cachedHead.texturePath.empty() &&
        (cachedHead.nextPathResolveAttempt == std::chrono::steady_clock::time_point {} ||
         now >= cachedHead.nextPathResolveAttempt)) {
        cachedHead.texturePath = PlayerHeadCache::getTexturePath(entry.skin);
        if (cachedHead.texturePath.empty()) {
            cachedHead.pathResolveAttempts++;
            std::chrono::milliseconds delay = cachedHead.pathResolveAttempts <= 1   ? std::chrono::milliseconds(500)
                                              : cachedHead.pathResolveAttempts == 2 ? std::chrono::milliseconds(1000)
                                              : cachedHead.pathResolveAttempts == 3 ? std::chrono::milliseconds(2000)
                                                                                    : std::chrono::milliseconds(10000);
            cachedHead.nextPathResolveAttempt = now + delay;
        } else {
            cachedHead.pathResolveAttempts = 0;
            cachedHead.nextPathResolveAttempt = {};
        }
    }
    if (cachedHead.texturePath.empty()) return;

    if (!cachedHead.texture.textureData &&
        (cachedHead.nextTextureLoadAttempt == std::chrono::steady_clock::time_point {} ||
         now >= cachedHead.nextTextureLoadAttempt)) {
        dc.renderCtx->getTexture(&cachedHead.texture,
                                 SDK::ResourceLocation(cachedHead.texturePath, SDK::ResourceFileSystem::Raw), true);
        if (!cachedHead.texture.textureData) {
            cachedHead.textureLoadAttempts++;
            std::chrono::milliseconds delay = cachedHead.textureLoadAttempts <= 1   ? std::chrono::milliseconds(500)
                                              : cachedHead.textureLoadAttempts == 2 ? std::chrono::milliseconds(1000)
                                              : cachedHead.textureLoadAttempts == 3 ? std::chrono::milliseconds(2000)
                                                                                    : std::chrono::milliseconds(10000);
            cachedHead.nextTextureLoadAttempt = now + delay;
        } else {
            cachedHead.textureLoadAttempts = 0;
            cachedHead.nextTextureLoadAttempt = {};
        }
    }

    if (!cachedHead.texture.textureData) return;

    dc.renderCtx->drawImage(cachedHead.texture, { bounds.left * dc.guiScale, bounds.top * dc.guiScale },
                            { bounds.getWidth() * dc.guiScale, bounds.getHeight() * dc.guiScale }, { 0.f, 0.f },
                            { 1.f, 1.f });
    dc.renderCtx->flushImages(d2d::Colors::WHITE, 1.f, SDK::HashedString("ui_textured_and_glcolor_sprite"));
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

    if (cachedActivePlayerNames.empty()) {
        refreshActivePlayerNames(level);
    }
    if (Latite::get().getNameTagCache().recordRenderedNameTag(*tag, cachedActivePlayerNames)) {
        rowsDirty = true;
    }
}

void TabList::onTick(Event& evG) {
    auto& ev = static_cast<TickEvent&>(evG);
    auto* level = ev.getLevel();
    syncNameTagCache(level);
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
    if (!screenView || !screenView->visualTree || !screenView->visualTree->rootControl ||
        screenView->visualTree->rootControl->name != "hud_screen")
        return;

    auto plr = SDK::ClientInstance::get()->getLocalPlayer();
    if (!plr) return;

    MCDrawUtil dc { ev.getUIRenderContext(), Latite::get().getFont() };
    dc.setImmediate(false);
    auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
    if (!lvl || !lvl->getPlayerList()) return;

    size_t size = lvl->getPlayerList()->size();
    if (cachedRows.size() != size || cachedActivePlayerNames.empty()) {
        syncNameTagCache(lvl);
    }
    if (rowsDirty) {
        rebuildRows(lvl);
    }
    size = cachedRows.size();

    float textP = getFloatOrDefault(textSizeS, 20.f);

    std::wstring txt;
    if (SDK::RakNetConnector::get() && SDK::RakNetConnector::get()->featuredServer.size() > 0) {
        txt = util::StrToWStr(SDK::RakNetConnector::get()->featuredServer);
    } else {
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

    updateLayoutMeasurements(dc, txt, textP, rowTextOffset);

    float sectionSize = std::max(1.f, cachedLongestText);

    size_t maxPerTab = 16;
    int numTabs = std::max(1, static_cast<int>(std::ceil(static_cast<float>(size) / static_cast<float>(maxPerTab))));

    float oY = sectionHeight;

    float calcWidth = sectionSize * static_cast<float>(numTabs);
    float calcHeight = sectionHeight * ((static_cast<float>(size) > maxPerTab) ? maxPerTab : static_cast<float>(size));

    auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;
    const Vec2 offset = { ss.x / 2.f - calcWidth / 2.f, 20.f };

    dc.fillRectangle({ offset.x, offset.y, offset.x + calcWidth, offset.y + calcHeight + oY },
                     backgroundColor.getMainColor());
    dc.drawRectangle({ offset.x, offset.y, offset.x + calcWidth, offset.y + calcHeight + oY },
                     d2d::Color(backgroundColor.getMainColor()).asAlpha(1.f), 2.f);

    dc.drawText({ offset.x, offset.y, offset.x + calcWidth, offset.y + oY }, txt, textColor.getMainColor(), font, textP,
                DWRITE_TEXT_ALIGNMENT_CENTER);
    dc.flush(false, true);

    auto rowRect = [&](size_t rowIndex) {
        size_t column = rowIndex / maxPerTab;
        size_t rowInColumn = rowIndex % maxPerTab;
        float x = sectionSize * static_cast<float>(column);
        float y = oY + sectionHeight * static_cast<float>(rowInColumn);
        return d2d::Rect { offset.x + x, offset.y + y, offset.x + x + sectionSize, offset.y + y + sectionHeight };
    };

    std::unordered_map<std::string, SDK::PlayerListEntry*> currentEntries;
    currentEntries.reserve(lvl->getPlayerList()->size());
    for (auto& ent : *lvl->getPlayerList()) {
        currentEntries.emplace(ent.second.name, &ent.second);
    }

    for (size_t rowIndex = 0; rowIndex < cachedRows.size(); ++rowIndex) {
        auto& row = cachedRows[rowIndex];
        auto entry = currentEntries.find(row.playerName);
        if (entry == currentEntries.end() || !entry->second) continue;

        d2d::Rect rc = rowRect(rowIndex);
        d2d::Rect headRect = { rc.left + headInset, rc.top + ((sectionHeight - headSize) * 0.5f),
                               rc.left + headInset + headSize,
                               rc.top + ((sectionHeight - headSize) * 0.5f) + headSize };
        drawPlayerHead(dc, *entry->second, row.playerName, headRect);
    }

    for (size_t rowIndex = 0; rowIndex < cachedRows.size(); ++rowIndex) {
        auto& row = cachedRows[rowIndex];
        d2d::Rect rc = rowRect(rowIndex);

        d2d::Rect textRect = rc;
        textRect.left += rowTextOffset;

        dc.drawText(textRect, row.displayNameWide, textColor.getMainColor(), font, textP, DWRITE_TEXT_ALIGNMENT_LEADING,
                    DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);
    }

    dc.flush();
}

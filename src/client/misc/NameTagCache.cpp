#include "pch.h"
#include "NameTagCache.h"
#include <cctype>

bool NameTagCache::readFormatCode(std::string const& text, size_t index, char& code, size_t& codeSize) const {
    if (index + 1 < text.size() && static_cast<unsigned char>(text[index]) == 0xA7) {
        code = text[index + 1];
        codeSize = 2;
        return true;
    }
    if (index + 2 < text.size() && static_cast<unsigned char>(text[index]) == 0xC2 &&
        static_cast<unsigned char>(text[index + 1]) == 0xA7) {
        code = text[index + 2];
        codeSize = 3;
        return true;
    }
    return false;
}

bool NameTagCache::isColorCode(char code) const {
    return std::isxdigit(static_cast<unsigned char>(code));
}

std::string NameTagCache::stripFormatCodes(std::string const& text) const {
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

bool NameTagCache::hasFormatCode(std::string const& text) const {
    for (size_t i = 0; i < text.size();) {
        char code = 0;
        size_t codeSize = 0;
        if (readFormatCode(text, i, code, codeSize)) return true;
        i++;
    }
    return false;
}

int NameTagCache::colorCodeIndex(char code) const {
    if (code >= '0' && code <= '9') return code - '0';
    if (code >= 'a' && code <= 'f') return code - 'a' + 10;
    if (code >= 'A' && code <= 'F') return code - 'A' + 10;
    return 16;
}

int NameTagCache::getFirstColorSortIndex(std::string const& text) const {
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

std::string NameTagCache::colorizedPlayerName(std::string const& playerName, std::string const& nameTag) const {
    const size_t namePos = stripFormatCodes(nameTag).find(playerName);
    if (namePos == std::string::npos) return playerName;

    return colorizedPlayerNameAt(playerName, nameTag, namePos);
}

std::string NameTagCache::colorizedPlayerNameAt(std::string const& playerName, std::string const& nameTag,
                                                size_t namePos) const {
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

std::optional<std::string> NameTagCache::formattedNameFromCachedTags(std::string const& playerName) const {
    for (auto const& [_, nameTag] : networkNameTags) {
        std::string rowName = colorizedPlayerName(playerName, nameTag);
        if (hasFormatCode(rowName)) return rowName;
    }
    return std::nullopt;
}

void NameTagCache::recordNetworkNameTag(uint64_t runtimeId, std::string const& nameTag) {
    if (runtimeId == 0) return;
    if (nameTag.empty()) {
        if (networkNameTags.erase(runtimeId) > 0) {
            networkNameTagsRevision++;
        }
        return;
    }

    auto found = networkNameTags.find(runtimeId);
    if (found != networkNameTags.end() && found->second == nameTag) return;

    networkNameTags[runtimeId] = nameTag;
    networkNameTagsRevision++;
}

bool NameTagCache::recordRenderedNameTag(std::string const& nameTag,
                                         std::unordered_set<std::string> const& playerNames) {
    if (!hasFormatCode(nameTag)) return false;

    bool changed = false;
    std::string strippedNameTag = stripFormatCodes(nameTag);

    for (auto const& playerName : playerNames) {
        size_t namePos = strippedNameTag.find(playerName);
        if (namePos == std::string::npos) continue;

        std::string formattedName = colorizedPlayerNameAt(playerName, nameTag, namePos);
        if (hasFormatCode(formattedName)) {
            auto found = formattedPlayerNames.find(playerName);
            if (found == formattedPlayerNames.end() || found->second != formattedName) {
                formattedPlayerNames[playerName] = formattedName;
                changed = true;
            }
        }
    }

    return changed;
}

std::optional<std::string> NameTagCache::getFormattedPlayerName(std::string const& playerName) const {
    auto it = formattedPlayerNames.find(playerName);
    if (it == formattedPlayerNames.end() || it->second.empty()) return std::nullopt;
    return it->second;
}

bool NameTagCache::updateFormattedPlayerNames(std::unordered_set<std::string> const& playerNames) {
    bool changed = false;

    for (auto it = formattedPlayerNames.begin(); it != formattedPlayerNames.end();) {
        if (playerNames.contains(it->first))
            ++it;
        else {
            it = formattedPlayerNames.erase(it);
            changed = true;
        }
    }

    for (auto const& playerName : playerNames) {
        auto formattedName = formattedNameFromCachedTags(playerName);
        if (formattedName) {
            auto found = formattedPlayerNames.find(playerName);
            if (found == formattedPlayerNames.end() || found->second != *formattedName) {
                formattedPlayerNames[playerName] = *formattedName;
                changed = true;
            }
        }
    }

    return changed;
}

void NameTagCache::clearNetworkNameTags() {
    if (!networkNameTags.empty() || !formattedPlayerNames.empty()) {
        networkNameTagsRevision++;
    }
    networkNameTags.clear();
    formattedPlayerNames.clear();
}

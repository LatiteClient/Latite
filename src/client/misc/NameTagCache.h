#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

class NameTagCache {
public:
	void recordNetworkNameTag(uint64_t runtimeId, std::string const& nameTag);
	void recordRenderedNameTag(std::string const& nameTag, std::unordered_set<std::string> const& playerNames);
	std::optional<std::string> getFormattedPlayerName(std::string const& playerName) const;
	void updateFormattedPlayerNames(std::unordered_set<std::string> const& playerNames);
	void clearNetworkNameTags();
	int getFirstColorSortIndex(std::string const& text) const;
	bool hasFormatCode(std::string const& text) const;
	std::string stripFormatCodes(std::string const& text) const;

private:
	std::unordered_map<uint64_t, std::string> networkNameTags;
	std::unordered_map<std::string, std::string> formattedPlayerNames;

	bool readFormatCode(std::string const& text, size_t index, char& code, size_t& codeSize) const;
	bool isColorCode(char code) const;
	int colorCodeIndex(char code) const;
	std::string colorizedPlayerName(std::string const& playerName, std::string const& nameTag) const;
	std::optional<std::string> formattedNameFromCachedTags(std::string const& playerName) const;
};

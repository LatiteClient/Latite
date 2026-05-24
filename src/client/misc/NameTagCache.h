#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_set>

class NameTagCache {
public:
	static void recordNetworkNameTag(uint64_t runtimeId, std::string const& nameTag);
	static std::optional<std::string> getNetworkNameTag(uint64_t runtimeId);
	static void retainNetworkNameTags(std::unordered_set<uint64_t> const& runtimeIds);
	static void clearNetworkNameTags();
};

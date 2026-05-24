#include "pch.h"
#include "NameTagCache.h"
#include <unordered_map>

namespace {
	std::unordered_map<uint64_t, std::string> networkNameTags;
}

void NameTagCache::recordNetworkNameTag(uint64_t runtimeId, std::string const& nameTag) {
	if (runtimeId == 0) return;
	if (nameTag.empty()) {
		networkNameTags.erase(runtimeId);
		return;
	}
	networkNameTags[runtimeId] = nameTag;
}

std::optional<std::string> NameTagCache::getNetworkNameTag(uint64_t runtimeId) {
	auto it = networkNameTags.find(runtimeId);
	if (it == networkNameTags.end() || it->second.empty()) return std::nullopt;
	return it->second;
}

void NameTagCache::retainNetworkNameTags(std::unordered_set<uint64_t> const& runtimeIds) {
	for (auto it = networkNameTags.begin(); it != networkNameTags.end();) {
		if (!runtimeIds.contains(it->first)) {
			it = networkNameTags.erase(it);
			continue;
		}
		++it;
	}
}

void NameTagCache::clearNetworkNameTags() {
	networkNameTags.clear();
}

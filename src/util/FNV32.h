#pragma once
#include <cstdint>
#include <iostream>
#include <array>
#include <cstddef>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

namespace util {
	constexpr uint32_t FNV_PRIME = 16777619u;
	constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;

	namespace detail {
		inline constexpr uint32_t fnv1a_32_const(char const* s, std::size_t count) {
			return count ? (fnv1a_32_const(s, count - 1) ^ s[count - 1]) * FNV_PRIME : FNV_OFFSET_BASIS;
		}
	}

	inline uint32_t fnv1a_32(std::string const& str) {
		uint32_t hash = FNV_OFFSET_BASIS;

		for (char c : str) {
			hash ^= static_cast<uint32_t>(c);
			hash *= FNV_PRIME;
		}

		return hash;
	}
}

constexpr uint32_t operator"" _hash(char const* s, std::size_t count) {
	return util::detail::fnv1a_32_const(s, count);
}

#define TOHASH(x) #x##_hash

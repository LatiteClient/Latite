#pragma once
#include <cstdint>
#include <iostream>

namespace util {
	namespace detail {
		inline constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count) {
			return count ? (fnv1a_32(s, count - 1) ^ s[count - 1]) * 16777619u : 2166136261u;
		}
	}
}

constexpr std::uint32_t operator"" _hash(char const* s, std::size_t count) {
	return util::detail::fnv1a_32(s, count);
}

#define TOHASH(x) #x##_hash

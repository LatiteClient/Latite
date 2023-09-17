#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>

namespace util {
	constexpr uint32_t FNV_PRIME = 16777619u;
	constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;

	constexpr uint64_t FNV_PRIME_64 = 0x100000001b3;
	constexpr uint64_t FNV_OFFSET_BASIS_64 = 0xcbf29ce484222325;

	// TODO: find a better way to do this without recursion (it slows down builds)
	namespace detail {
		inline constexpr uint32_t fnv1a_32_const(char const* s, std::size_t count) {
			return count ? (fnv1a_32_const(s, count - 1) ^ s[count - 1]) * FNV_PRIME : FNV_OFFSET_BASIS;
		}

		inline constexpr uint64_t fnv1a_64_const(char const* s, std::size_t count) {
			return count ? (fnv1a_64_const(s, count - 1) ^ s[count - 1]) * FNV_PRIME_64 : FNV_OFFSET_BASIS_64;
		}

		inline constexpr uint64_t fnv1_64_const(char const* s, std::size_t count) {
			uint64_t hash = FNV_OFFSET_BASIS_64;
			for (std::size_t i = 0; i < count; ++i) {
				hash *= FNV_PRIME_64;
				hash ^= static_cast<uint64_t>(s[i]);
			}
			return hash;
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

	inline uint64_t fnv1a_64(std::string const& str) {
		uint64_t hash = FNV_OFFSET_BASIS_64;

		for (char c : str) {
			hash ^= static_cast<uint64_t>(c);
			hash *= FNV_PRIME_64;
		}

		return hash;
	}

	// TODO: test this
	inline uint64_t fnv1a_64w(std::wstring const& str) {
		uint64_t hash = FNV_OFFSET_BASIS_64;

		for (wchar_t c : str) {
			hash ^= static_cast<uint64_t>(c);
			hash *= FNV_PRIME_64;
		}

		return hash;
	}
}

constexpr uint32_t operator"" _fnv32(char const* s, std::size_t count) {
	return util::detail::fnv1a_32_const(s, count);
}

constexpr uint64_t operator"" _fnv64(char const* s, std::size_t count) {
	return util::detail::fnv1_64_const(s, count);
}

#define TOHASH(x) #x##_fnv32

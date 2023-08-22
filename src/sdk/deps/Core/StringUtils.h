#pragma once
#include <string>
#include <cstdint>

namespace SDK {
	class HashedString {
	public:
		int64_t hash;
	private:
		std::string string;
		void* idk;

	public:
		HashedString(int64_t hash, std::string text) : hash(hash), string(text.c_str()), idk(nullptr) {};

		std::string getString() {
			return string;
		}

		static uint64_t Hash(std::string const& str) {
			const uint64_t FNV_OFFSET_BASIS = 0xcbf29ce484222325;
			const uint64_t FNV_PRIME = 0x100000001b3;
			const uint8_t* bytes = reinterpret_cast<const uint8_t*>(str.c_str());
			uint64_t hash = FNV_OFFSET_BASIS;
			for (size_t i = 0; i < str.length(); ++i) {
				hash = hash ^ bytes[i];
				hash = hash * FNV_PRIME;
			}
			return hash;
		}

		HashedString(std::string const& str) : string(str.c_str()), idk(nullptr) {
			hash = Hash(str);
			string = str;
		}

		bool operator==(uint64_t right) {
			return right == hash;
		}

		bool operator==(std::string const& right) {
			return string == right;
		}

		bool operator!=(uint64_t right) {
			return !operator==(right);
		}

		bool operator!=(std::string const& right) {
			return !operator==(right);
		}
	};
}
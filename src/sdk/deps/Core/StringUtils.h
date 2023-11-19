#pragma once
#include <string>
#include <cstdint>
#include "util/FNV32.h"

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

		HashedString(std::string const& str) : string(str.c_str()), idk(nullptr) {
			hash = util::fnv1a_64(str);
			string = str;
		}

		HashedString(HashedString const& that) {
			this->string = that.string;
			this->hash = that.hash;
			this->idk = nullptr;
		}

		HashedString& operator=(HashedString const& right) {
			this->string = right.string;
			this->hash = right.hash;
			this->idk = nullptr;
			return *this;
		}

		HashedString() = default;

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
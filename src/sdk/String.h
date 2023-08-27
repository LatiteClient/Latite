#pragma once
#include <string>

template <size_t S>
// WARNING: This does not destroy itself.
class NonOwnedString {
private:

	union {
		char* ptr;
		char body[S] = {};
	};

public:
	size_t textSize;
private:
	size_t textSizeAligned;

public:
	void setString(const char* str) {
		size_t size = ::strlen(str);

		if (size < S) {
			::memcpy(this->body, str, size);
		}
		else {
			size_t allSize = size + 1;
			char* m = reinterpret_cast<char*>(::malloc(allSize));
			::memset(m, 0x0, allSize);
			::memcpy(m, str, allSize);
			this->ptr = m;
		}

		this->textSize = size;
		this->textSizeAligned = size | (S - 1);
	}

	std::string str() {
		return std::string(reinterpret_cast<std::string*>(this)->c_str()); // avoid double delete
	}

	inline const char* getCStr() const {
		if (textSize < S) {
			return body;
		}
		else return ptr;
	}

	void operator=(const char* right) {
		// Delete the old string first
		if (textSize >= S) {
			//free(ptr); This always causes a double deletion and I have no clue why
		}

		setString(right);
	}

	operator std::string() {
		return std::string(getCStr());
	}

	void release() {
		if (textSize >= S) {
			free(ptr);
		}
	}

	bool empty() { return textSize == 0; }
};

// WARNING: This does not destroy itself.
using String = NonOwnedString<16>;

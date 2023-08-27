#pragma once
#include <string>
#include <vector>

namespace SDK {
	class InputMapping {
	public:
		std::string name; // 0x0000
		int* value; //0x0020
		void* ptr1; //0x0028 ptr to 8 bytes more than value
		void* ptr2; //0x0030 same as ptr1
		int64_t bool1; //0x0038
	};

	class RemappingLayout {
		[[maybe_unused]] void* vtbl;

	public:
		std::vector<InputMapping> inputSettings; //0x0008
		std::vector<InputMapping> defaultSettings; // 0x0010

		int findValue(std::string const& name);
		int findValueFromOriginal(int val);
	}; //Size: 0x0400

	class KeyboardLayout : public RemappingLayout {
	public:
		int type;
	};
}
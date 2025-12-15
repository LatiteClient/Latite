#pragma once
#include <string>
#include <vector>

namespace SDK {
	class InputMapping {
	public:
		std::string name; // 0x00
		std::vector<int> keys; // 0x20
		bool allowRemap; //0x38

		int getFirstKey() const {
			return keys.at(0);
		}
	};

	class RemappingLayout {
		[[maybe_unused]] void* vtbl;

	public:
		std::vector<InputMapping> inputSettings; //0x0008
		std::vector<InputMapping> defaultSettings; // 0x0010
		char pad[0x10];

		int findValue(std::string const& name);
		int findValueFromOriginal(int val);
	}; //Size: 0x0400

	class KeyboardLayout : public RemappingLayout {
	public:
		int type;
	};
}
#include <string>

namespace SDK {
	class Font {
	private:
		uintptr_t* vtable;
	public:
		int getLineLength(std::string_view str, float fontSize, bool showColorSymbol = false) {
			using Fn = int(__thiscall*)(Font*, std::string_view, float, bool);
			return reinterpret_cast<Fn>(vtable[0x6])(this, str, fontSize, showColorSymbol);
		}

		float getLineHeight() {
			return reinterpret_cast<float(*)(Font*)>(vtable[0x7])(this);
		}
	};
}
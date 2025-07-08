#include <string>

namespace SDK {
	class Font {
	private:
		uintptr_t* vtable;
	public:
		float getLineLength(std::string const& str, float textSize, bool colorCodeSymbols = false) {
			return reinterpret_cast<float(*)(Font*, std::string const&, float, bool)>(vtable[0x6])(this, str, textSize, colorCodeSymbols);
		}

		float getLineHeight() {
			return reinterpret_cast<float(*)(Font*)>(vtable[0x7])(this);
		}
	};
}
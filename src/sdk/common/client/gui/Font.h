#include <string>

namespace SDK {
	class Font {
	private:
		uintptr_t* vtable;
	public:
		float getLineLength(std::string const& str, float textSize, bool unk = false /*this may be related to trailing/leading whitespace*/) {
			return reinterpret_cast<float(*)(Font*, std::string const&, float, bool)>(vtable[0x6])(this, str, textSize, unk);
		}

		float getLineHeight() {
			return reinterpret_cast<float(*)(Font*)>(vtable[0x7])(this);
		}
	};
}
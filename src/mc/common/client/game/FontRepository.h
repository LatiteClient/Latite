#pragma once
#include "mc/Util.h"

namespace SDK {
	class FontRepository {
	public:
		CLASS_FIELD(std::vector<std::shared_ptr<class Font>>, fontList, 0x40);
	
		class Font* getSmoothFont() {
			return fontList[7].get();
		}
	};
}
#pragma once
#include <memory>
#include "sdk/Util.h"

namespace SDK {
	class MaterialPtr {
	public:
		std::shared_ptr<MaterialPtr> self;
		HashedString name;

		// TODO: construct own materials
		static MaterialPtr* getUIColor();
		static MaterialPtr* getUITextureAndColor();
	};
}

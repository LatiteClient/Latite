#pragma once
#include <memory>
#include "sdk/SDKBase.h"

namespace SDK {
	class MaterialPtr : public Incomplete {
	public:
		std::shared_ptr<MaterialPtr> self;
		void* name[3]; // TODO: HashedString

		// TODO: construct own materials
		static MaterialPtr* getUIColor();
	};
}
#pragma once
#include <memory>
#include "mc/Util.h"

namespace SDK {
	class RenderMaterialInfo : std::enable_shared_from_this<RenderMaterialInfo> {
		HashedString name;
		std::unique_ptr<void*> renderMaterial; // unique_ptr<mce::RenderMaterial>
	};

	class MaterialPtr {
	public:
		std::shared_ptr<RenderMaterialInfo> self;

		MaterialPtr(MaterialPtr const& that) {
			this->self = that.self;
		}

		// TODO: construct own materials
		static MaterialPtr* getUIColor();
		static MaterialPtr* getUITextureAndColor();
		static MaterialPtr* createMaterial(const HashedString& name);
		static MaterialPtr* getSelectionBoxMaterial();
		static MaterialPtr* getSelectionOverlayMaterial();
	};
}

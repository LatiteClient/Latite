#pragma once
#include "sdk/common/client/renderer/HudPlayerRenderer.h"
#include <string>
#include "util/LMath.h"
#include "util/DxUtil.h"
#include "sdk/Util.h"
#include <vector>
#include <memory>
#include <functional>

namespace SDK {
	class UIComponent
	{
	public:
		class UIControl* control; //0x0008

		virtual void Function0();
	}; //Size: 0x00A0

	class TextComponent : public UIComponent {
	public:
		CLASS_FIELD(std::string, variableToText, 0x0130);
		CLASS_FIELD(std::string, resolvedText, 0x0188);
		CLASS_FIELD(std::string, resolvedText2, 0x01A8);
		CLASS_FIELD(Vec2, controlBounds, 0x01C8);
		CLASS_FIELD(std::string, variableToText2, 0x01D8);
		CLASS_FIELD(std::string, resolvedText3, 0x01F8);
		CLASS_FIELD(std::string, lang, 0x0218);
		CLASS_FIELD(std::string, font, 0x0238);
		CLASS_FIELD(std::string, resolvedText4, 0x0258);
	};

	class CustomRenderComponent : UIComponent { // 4 in list
		char pad[8];
	public:
		HudPlayerRenderer* rend;
	};

	static_assert(offsetof(CustomRenderComponent, rend) == 0x18);
	class UIControl
	{
	public:
		char pad_0000[24]; //0x0000
	public:
		std::string name; //0x0018
	private:
		char pad_0038[8]; //0x0038
	public:
		Vec2 bounds; //0x0040
	private:
		char pad_0048[36]; //0x0048
	public:
		int32_t controlState; //0x006C
	private:
		char pad_0070[8]; //0x0070
	public:
		Vec2 position; //0x0078
	private:
		char pad_0080[8]; //0x0080
	public:
		class UIPropertyBag* uiPropertyBag; //0x0088
		std::shared_ptr<UIControl> parent; //0x0090
		std::vector<std::shared_ptr<UIControl>> children; //0x00A0
	private:
		char pad_00B8[8]; //0x00B8
	public:
		std::vector<UIComponent*> uiComponents; //0x00C0 supposed to be vector of UIComponent
	private:
		char pad_00C8[40 - 16]; //0x00C8
		class UIControl* unknownControl; //0x00F0  I dont even think this exists
		char pad_00F8[56]; //0x00F8
	public:
		std::shared_ptr<UIControl> topControl; //0x0130 for example, start_screen
	private:
		char pad_0140[8]; //0x0140
		int32_t refCount; //0x0148
		bool hasInitialized;
	public:
		std::shared_ptr<UIControl> findControl(const char* name) {
			for (size_t i = 0; i < children.size(); i++) {
				auto ctrl = children[i];
				if (ctrl->name == name) {
					return ctrl;
				}
			}
			return nullptr;
		}

		UIControl* findFirstDescendantWithName(std::string const& targetName) {
			if (name == targetName) {
				return this;
			}
			for (size_t i = 0; i < children.size(); i++) {
				auto child = children[i];
				auto descendant = child->findFirstDescendantWithName(targetName);
				if (descendant != nullptr) {
					return descendant;
				}
			}
			return nullptr;
		}

		void getControls(std::function<void(std::shared_ptr<UIControl> control)> func) {
			for (size_t i = 0; i < children.size(); i++) {
				auto child = children[i];
				func(child);
			}
		}

		void getDescendants(std::function<void(std::shared_ptr<UIControl> control)> func, int* deep = nullptr) {
			if (children.size() == 0) {
				if (deep) *deep = 1;
			}

			for (size_t i = 0; i < children.size(); i++) {
				auto child = children[i];
				func(child);
				child->getDescendants(func, deep);
			}
		}

		d2d::Rect getRect() {
			return { position, position + bounds };
		}

		void updatePos();
	};
}

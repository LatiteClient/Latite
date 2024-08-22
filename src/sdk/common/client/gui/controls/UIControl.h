#pragma once
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

	class TextComponent : public UIComponent { // TODO: needs checking
	public:
		CLASS_FIELD(std::string, variableToText, 0x0138);
		CLASS_FIELD(std::string, resolvedText, 0x0190);
		CLASS_FIELD(std::string, resolvedText2, 0x01B0);
		CLASS_FIELD(Vec2, controlBounds, 0x01D0);
		CLASS_FIELD(std::string, variableToText2, 0x01E0);
		CLASS_FIELD(std::string, resolvedText3, 0x0200);
		CLASS_FIELD(std::string, lang, 0x0220);
		CLASS_FIELD(std::string, font, 0x0240);
		CLASS_FIELD(std::string, resolvedText4, 0x0260);
	};

	class CustomRenderComponent : UIComponent { // 4 in list
		char pad[8];
	public:
		class HudPlayerRenderer* rend;
	};

	static_assert(offsetof(CustomRenderComponent, rend) == 0x18);
	class UIControl
	{
		char pad_0000[16]; //0x0000
	public:
		Vec2 position; //0x0010
	private:
		char pad_0018[8]; //0x0018
	public:
		std::string name; //0x0020
	private:
		char pad_0040[8]; //0x0040
	public:
		Vec2 bounds; //0x0048
	private:
		char pad_0050[32]; //0x0050
	public:
		int32_t controlState; //0x0070
		class UIPropertyBag* uiPropertyBag; //0x0078
		std::shared_ptr<UIControl> parent; //0x0080
		std::vector<std::shared_ptr<UIControl>> children; //0x0090
	private:
		char pad_00A8[8]; //0x00A8
	public:
		std::vector<UIComponent*> uiComponents; //0x00B0 supposed to be vector of UIComponent
	private:
		char pad_00C8[24]; //0x00C8
		class UIControl* unknownControl; //0x00E0  I dont even think this exists
		char pad_00E8[56]; //0x00E8
	public:
		std::shared_ptr<UIControl> topControl; //0x0120 for example, start_screen
	private:
		char pad_0140[8]; //0x0130
		int32_t refCount; //0x0138
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

		void getDescendants(std::function<void(std::shared_ptr<UIControl> control)> func) {
			for (size_t i = 0; i < children.size(); i++) {
				auto child = children[i];
				func(child);
				child->getDescendants(func);
			}
		}

		d2d::Rect getRect() {
			return { position, position + bounds };
		}

		void updatePos();
	};
}

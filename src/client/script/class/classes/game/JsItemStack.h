#pragma once
#include "../../JsClass.h"
#include "mc/common/world/Item.h"
#include "mc/common/world/ItemStack.h"
#include "JsItem.h"

class JsItemStack : public JsWrapperClass<SDK::ItemStack> {
protected:
	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsItemStack*>(callbackState);
		auto item = Get(arguments[0]);
		std::string add = std::format("{} (name={}, aux={})", util::WStrToStr(thi->name), item->item ? item->getHoverName() : "air", item->aux);
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}

	static JsValueRef CALLBACK getDisplayNameCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		auto thi = reinterpret_cast<JsItemStack*>(callbackState);
		auto item = Get(arguments[0]);
		return Chakra::MakeString(util::StrToWStr(item->getHoverName()));
	}

	static JsValueRef CALLBACK getCountCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		auto thi = reinterpret_cast<JsItemStack*>(callbackState);
		auto item = Get(arguments[0]);
		return Chakra::MakeInt(item->itemCount);
	}

	static JsValueRef CALLBACK getPickupTime(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		auto thi = reinterpret_cast<JsItemStack*>(callbackState);
		auto item = Get(arguments[0]);

		// double because of Y2K38 problem
		return Chakra::MakeDouble(static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(item->pickupTime.time_since_epoch()).count()));
	}
public:
	inline static const wchar_t* class_name = L"ItemStack";
	JsItemStack(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsNoConstructor, this);
	}

	JsValueRef construct(SDK::ItemStack* item, bool del) {
		auto obj = __super::construct(item, del);
		if (item->item) {
			Chakra::SetProperty(obj, L"item", this->owner->getClass<JsItem>()->construct(*item->item, false), true);
		}
		else {
			Chakra::SetProperty(obj, L"item", Chakra::GetNull(), true);
		}
		Chakra::SetPropertyNumber(obj, L"aux", static_cast<double>(item->aux), true);
		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, toStringCallback, XW("toString"), this);
		Chakra::DefineFunc(prototype, getDisplayNameCallback, XW("getDisplayName"), this);
		Chakra::DefineFunc(prototype, getCountCallback, XW("getCount"), this);
		Chakra::DefineFunc(prototype, getPickupTime, XW("getPickupTime"), this);
	};
};
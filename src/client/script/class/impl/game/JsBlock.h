#pragma once
#include "../../JsWrapperClass.h"
#include "sdk/common/world/level/block/Block.h"
;

class JsBlock : public JsWrapperClass<SDK::Block> {
protected:

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsBlock*>(callbackState);
		SDK::Block* block = Get(arguments[0]);

		std::string id = "air";

		if (block->legacyBlock) {
			id = block->legacyBlock->namespacedId;
		}

		std::string add = std::format("{} ({})", util::WStrToStr(thi->name), id);
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}
public:
	inline static const wchar_t* class_name = L"Block";
	JsBlock(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsNoConstructor, this);
	}

	JsValueRef construct(SDK::Block* block, bool del) {
		JsValueRef obj;
		if (del) {
			JS::JsCreateExternalObject(block, [](void* obj) {
				delete obj;
				}, &obj);
		}
		else {
			JS::JsCreateExternalObject(block, [](void*) {
				}, &obj);
		}
		JS::JsSetPrototype(obj, getPrototype());

		if (block->legacyBlock) {
			Chakra::SetPropertyString(obj, L"name", util::StrToWStr(block->legacyBlock->namespacedId));
			Chakra::SetPropertyString(obj, L"translateName", util::StrToWStr(block->legacyBlock->translateName.getString()));
		}
		else {
			Chakra::SetPropertyString(obj, L"name", L"air");
			Chakra::SetPropertyString(obj, L"translateName", L"tile.air");
		}
		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
	};
};
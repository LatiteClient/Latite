#pragma once
#include "../JsLibrary.h"
#include "../../JsScript.h"

class Network : public JsLibrary {
public:
	Network(class JsScript* owner) : JsLibrary(owner, L"network") {}

	JsValueRef initialize(JsValueRef parent) override;

	class NetAsyncOperation : public JsScript::AsyncOperation {
	public:
		std::wstring url = L"";

		int err = 0;
		std::optional<std::wstring> data = std::nullopt;

		virtual void getArgs() override {
			JsValueRef err;
			JS::JsIntToNumber(this->err, &err);
			this->args.push_back(err);
			if (data.has_value()) {
				JsValueRef data;
				JS::JsPointerToString(this->data.value().c_str(), this->data.value().size(), &data);
				this->args.push_back(data);
			}
		}

		NetAsyncOperation(JsValueRef callback, decltype(initFunc) initFunc, void* param)
			: JsScript::AsyncOperation(true, callback, initFunc, param), err(0), data(L"")
		{
		}
	};

	static JsValueRef CALLBACK get(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);

	static JsValueRef CALLBACK getSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
};
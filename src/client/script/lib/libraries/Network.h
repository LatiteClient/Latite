#pragma once
#include "../JsLibrary.h"
#include "../../JsPlugin.h"

#include <winrt/windows.storage.streams.h>

class Network : public JsLibrary {
public:
	Network(class JsScript* owner) : JsLibrary(owner, L"http") { }

	JsValueRef initialize(JsValueRef parent) override;

	class NetAsyncOperation : public JsScript::AsyncOperation {
	public:
		std::wstring url = L"";

		int err = 0;
		std::optional<winrt::Windows::Storage::Streams::IBuffer> data = std::nullopt;
		std::optional<std::wstring> winrtErr = std::nullopt;

		virtual void getArgs() override;

		NetAsyncOperation(JsValueRef callback, decltype(initFunc) initFunc, void* param)
			: JsScript::AsyncOperation(true, callback, initFunc, param), err(0), data(std::nullopt)
		{
		}
	};

private:

	virtual bool shouldInclude(std::wstring const& str) override {
		// deprecated name
		if (str == L"network") {
			return true;
		}

		return __super::shouldInclude(str);
	};

	static JsValueRef CALLBACK get(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);

	static JsValueRef CALLBACK getSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);

	static JsValueRef CALLBACK post(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);

	static JsValueRef CALLBACK postSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
};
#pragma once
#include "../JsLibrary.h"
#include <functional>
#include <thread>
#include "../../JsScript.h"

class Filesystem : public JsLibrary {
public:
	JsValueRef initialize(JsValueRef parent) override;
	Filesystem(JsScript* owner) : JsLibrary(owner, L"filesystem") {}
private:


	//template <typename T>
	//void runAsyncOperation(std::function<void(T)> func)

	class FSAsyncOperation : public JsScript::AsyncOperation {
	public:
		std::wstring path = L"";

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

		FSAsyncOperation(JsValueRef callback, decltype(initFunc) initFunc, void* param)
			: JsScript::AsyncOperation(true, callback, initFunc, param), err(0), data(L"")
		{
		}
	};

	std::wstring getPath(std::wstring relPath);

	static JsValueRef CALLBACK write(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK read(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK writeSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK readSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK existsSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK createDirectorySync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
};
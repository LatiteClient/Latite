#pragma once
#include "../JsLibrary.h"
#include <functional>
#include <thread>
#include "../JsPlugin.h"
#include <winrt/windows.storage.streams.h>

class Filesystem : public JsLibrary {
public:
	JsValueRef initialize(JsValueRef parent) override;
	Filesystem(JsScript* owner) : JsLibrary(owner, L"filesystem") {}
private:

	class FSAsyncOperation : public JsScript::AsyncOperation {
	public:
		

		std::wstring path = {};

		int err = 0;
		std::optional<std::vector<BYTE>> data = std::nullopt;
		bool outData = true;

		virtual void getArgs() override;

		FSAsyncOperation(JsValueRef callback, decltype(initFunc) initFunc, void* param)
			: JsScript::AsyncOperation(true, callback, initFunc, param)
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
	static JsValueRef CALLBACK append(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK appendSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK deleteFile(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK readdirSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK moveSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	
};
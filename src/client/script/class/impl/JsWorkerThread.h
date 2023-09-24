#pragma once
#include "../JsWrapperClass.h"

class JsWorkerThread : public JsWrapperClass<std::thread> {
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsWorkerThread*>(callbackState);
		throw thi->errNoConstruct();
	}

	static JsValueRef CALLBACK loadFile(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static void threadMain(std::wstring script);
public:

	inline static const wchar_t* class_name = L"Thread";

	JsWorkerThread(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsConstructor, this);
		Chakra::DefineFunc(constructor, loadFile, L"loadFile", this);
	}

	JsValueRef construct(std::thread* thread) {
		JsValueRef obj;
		JS::JsCreateExternalObject(thread, [](void* obj) {
			reinterpret_cast<std::thread*>(obj)->join();
			delete reinterpret_cast<std::thread*>(obj);
			}, &obj);
		JS::JsSetPrototype(obj, getPrototype());
		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, this->defaultToString, L"toString", this);
	};

	static std::thread& ToThread(JsValueRef obj) {
		std::thread* thread = nullptr;
		JS::JsGetExternalData(obj, reinterpret_cast<void**>(&thread));
		return *thread;
	}
};
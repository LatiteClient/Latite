#include "pch.h"
#include "JsWorkerThread.h"

JsValueRef JsWorkerThread::loadFile(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto thi = reinterpret_cast<JsWorkerThread*>(callbackState);
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return JS_INVALID_REFERENCE;

	auto path = util::GetLatitePath() / "Scripts" / thi->owner->relFolderPath / Chakra::GetString(arguments[1]);
	std::ifstream ifs;
	ifs.open(path);
	if (ifs.fail()) {
		Chakra::ThrowError(L"Could not open file " + path.wstring() + L" for reading");
		return JS_INVALID_REFERENCE;
	}

	auto thread = new std::thread(threadMain);
	return thi->construct(thread);
}

void JsWorkerThread::threadMain(std::wstring script) {
	JsRuntimeHandle handle;
	JsContextRef context;
	JsValueRef result;

	JS::JsCreateRuntime(JsRuntimeAttributeDisableBackgroundWork, nullptr, &handle);
	JS::JsCreateContext(handle, &context);

	JS::JsRunScript(script.c_str(), util::fnv1a_64w(script), L"<worker thread>", &result);

	JS::JsRelease(result, nullptr);
	JS::JsCollectGarbage(handle);
	JS::JsDisposeRuntime(handle);
}

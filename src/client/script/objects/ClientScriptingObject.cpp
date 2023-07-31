#include "ClientScriptingObject.h"
#include "util/Util.h"
#include "util/ChakraUtil.h"
#include "client/Latite.h"
#include "client/script/ScriptManager.h"

JsValueRef ClientScriptingObject::registerEventCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 3)) return undefined;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsFunction} })) return undefined;

	const wchar_t* myS;
	size_t sze;
	JS::JsStringToPointer(arguments[1], &myS, &sze);
	std::wstring wstr(myS);

	for (auto& lis : Latite::getScriptManager().eventListeners) {
		if (lis.first == wstr) {
			JsContextRef ct;
			JS::JsGetCurrentContext(&ct);
			JS::JsAddRef(arguments[2], nullptr);
			lis.second.push_back({ arguments[2], ct });
			return undefined;
		}
	}

	Chakra::ThrowError(L"Invalid event " + wstr);
	return undefined;
}

void ClientScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, registerEventCallback, L"on");
}

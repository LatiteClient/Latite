#include "OSScriptingObject.h"

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/windows.system.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <WinUser.h>

#include <Windows.h>
#include <Mmsystem.h>

JsValueRef OSScriptingObject::openUri(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsString }})) return JS_INVALID_REFERENCE;
	
	auto uri = winrt::Windows::Foundation::Uri(Chakra::GetString(arguments[1]));
	winrt::Windows::System::Launcher::LaunchUriAsync(uri);
	return Chakra::GetUndefined();
}

JsValueRef OSScriptingObject::playSound(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsString } })) return JS_INVALID_REFERENCE;

	auto obj = reinterpret_cast<OSScriptingObject*>(callbackState);

	std::filesystem::path path = Chakra::GetString(arguments[1]);
	try {
		if (!std::filesystem::exists(path)) {
			path = JsScript::getThis()->getPlugin()->getPath() / path;
		}
	}
	catch (...) {}


	PlaySoundW(path.wstring().c_str(), NULL, SND_FILENAME | SND_ASYNC);
	return Chakra::GetUndefined();
}

void OSScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, openUri, XW("openUri"), this);
	Chakra::DefineFunc(object, playSound, XW("playSound"), this);
}

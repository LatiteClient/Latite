#include "OSScriptingObject.h"

#include <WinUser.h>
#include <shellapi.h>

#include <Windows.h>
#include <Mmsystem.h>

JsValueRef OSScriptingObject::openUri(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                      unsigned short argCount, void* callbackState) {
    if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
    if (!Chakra::VerifyParameters({ { arguments[1], JsString } })) return JS_INVALID_REFERENCE;

    auto uri = Chakra::GetString(arguments[1]);
    auto result = ShellExecuteW(nullptr, L"open", uri.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        Chakra::ThrowError(L"Could not open URI");
    }

    return Chakra::GetUndefined();
}

JsValueRef OSScriptingObject::playSound(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                        unsigned short argCount, void* callbackState) {
    if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
    if (!Chakra::VerifyParameters({ { arguments[1], JsString } })) return JS_INVALID_REFERENCE;

    auto obj = reinterpret_cast<OSScriptingObject*>(callbackState);

    std::filesystem::path path = Chakra::GetString(arguments[1]);
    try {
        if (!std::filesystem::exists(path)) {
            path = JsScript::getThis()->getPlugin()->getPath() / path;
        }
    } catch (...) {}

    PlaySoundW(path.wstring().c_str(), NULL, SND_FILENAME | SND_ASYNC);
    return Chakra::GetUndefined();
}

void OSScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
    Chakra::DefineFunc(object, openUri, L"openUri", this);
    Chakra::DefineFunc(object, playSound, L"playSound", this);
}

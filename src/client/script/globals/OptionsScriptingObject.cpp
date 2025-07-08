#include "pch.h"
#include "OptionsScriptingObject.h"
#include <mc/common/client/game/Options.h>

JsValueRef OptionsScriptingObject::getPlayerViewPerspective(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
    return Chakra::MakeInt(SDK::ClientInstance::get()->getOptions()->getPlayerViewPerspective());
}

JsValueRef OptionsScriptingObject::setPlayerViewPerspective(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
    if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
    if (!Chakra::VerifyParameters({ {arguments[1], JsNumber} })) return JS_INVALID_REFERENCE;

    int perspective = Chakra::GetInt(arguments[1]);

    if (perspective > 0 && perspective < 3) {
        SDK::ClientInstance::get()->getOptions()->setPlayerViewPerspective(perspective);
        return Chakra::GetUndefined();
    }

    Chakra::ThrowError(XW("Perspective out of bounds [0-2]"));
    return Chakra::GetUndefined();
}

void OptionsScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
    Chakra::DefineFunc(object, getPlayerViewPerspective, XW("getPlayerViewPerspective"));
    Chakra::DefineFunc(object, setPlayerViewPerspective, XW("setPlayerViewPerspective"));
}

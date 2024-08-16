#include "pch.h"
#include "JsTextModuleClass.h"

JsValueRef JsTextModuleClass::getLastTextCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
    JsModule* tm = Get(arguments[0]);
    if (tm->isTextual()) {
        return Chakra::MakeString(reinterpret_cast<TextModule*>(tm)->getLastText());
    }

    Chakra::ThrowError(XW("Invalid text module"));
    return JS_INVALID_REFERENCE;
}

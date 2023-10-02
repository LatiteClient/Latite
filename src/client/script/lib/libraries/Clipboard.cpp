#include "pch.h"
#include "Clipboard.h"

JsValueRef Clipboard::initialize(JsValueRef parent) {
    JsValueRef obj;
    JS::JsCreateObject(&obj);

    Chakra::DefineFunc(obj, get, L"get", this);
    Chakra::DefineFunc(obj, getBitmap, L"getBitmap", this);
    Chakra::DefineFunc(obj, set, L"set", this);
    return obj;
}

JsValueRef Clipboard::get(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
    return Chakra::MakeString(util::GetClipboardText());
}

JsValueRef Clipboard::set(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
    if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
    if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return JS_INVALID_REFERENCE;

    util::SetClipboardText(Chakra::GetString(arguments[1]));
    return Chakra::GetUndefined();
}

JsValueRef Clipboard::getBitmap(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
    

    if (!OpenClipboard(nullptr)) {
        return Chakra::GetNull();
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return Chakra::GetNull();
    }

    BYTE* data = static_cast<BYTE*>(GlobalLock(hData));
    if (data == nullptr) {
        CloseClipboard();
        return Chakra::GetNull();
    }

    size_t size = GlobalSize(hData);

    JsValueRef arr;
    JS::JsCreateTypedArray(JsTypedArrayType::JsArrayTypeUint8, nullptr, 0, size, &arr);

    BYTE* chakraData;
    JS::JsGetTypedArrayStorage(arr, &chakraData, nullptr, nullptr, nullptr);

    for (size_t i = 0; i < size; ++i) {
        chakraData[i] = data[i];
    }

    // Release the lock
    GlobalUnlock(hData);


    // Release the clipboard
    CloseClipboard();

    return arr;
}

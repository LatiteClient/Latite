#pragma once

#include "../JsWrapperClass.h"
#include "client/script/JsEvented.h"

#include <atomic>
#include <memory>
#include <winhttp.h>

struct WebSocketState {
    HINTERNET session = nullptr;
    HINTERNET connection = nullptr;
    std::atomic<HINTERNET> socket = nullptr;

    void close();
    ~WebSocketState();
};

struct WebSocketHolder : JsEvented {
    inline static std::wstring_view receiveEventId = L"receive";
    inline static std::wstring_view closeEventId = L"close";

    std::shared_ptr<WebSocketState> state;
    bool binary = false;

    WebSocketHolder(std::shared_ptr<WebSocketState> socketState, bool binaryMessages);
    ~WebSocketHolder();
};

class JsWebSocketClass final : public JsWrapperClass<WebSocketHolder> {
    static std::shared_ptr<WebSocketHolder> getHolder(JsValueRef object);

    static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                             unsigned short argCount, void* callbackState);
    static JsValueRef CALLBACK send(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                    unsigned short argCount, void* callbackState);
    static JsValueRef CALLBACK close(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                     unsigned short argCount, void* callbackState);
    static JsValueRef CALLBACK on(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                  unsigned short argCount, void* callbackState);
    static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                                unsigned short argCount, void* callbackState) {
        return Chakra::MakeString(L"[object WebSocket]");
    }

public:
    inline static const wchar_t* class_name = L"WebSocket";

    JsWebSocketClass(JsScript* owner) : JsWrapperClass(owner, class_name) {
        createConstructor(jsConstructor, this);
    }

    JsValueRef construct(std::shared_ptr<WebSocketHolder> holder);

    void prepareFunctions() override {
        Chakra::DefineFunc(prototype, send, L"send", this);
        Chakra::DefineFunc(prototype, close, L"close", this);
        Chakra::DefineFunc(prototype, on, L"on", this);
        Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
    }
};

#pragma once
#include "../JsLibrary.h"
#include "../JsPlugin.h"

#include "util/HttpClient.h"

class Network : public JsLibrary {
public:
    Network(class JsScript* owner)
        : JsLibrary(owner, L"http") {}

    JsValueRef initialize(JsValueRef parent) override;

    class NetAsyncOperation : public JsScript::AsyncOperation {
    public:
        std::wstring url = L"";

        int status = 0;
        std::optional<std::vector<std::uint8_t>> body;
        std::optional<std::wstring> error;
        http::Headers headers;

        virtual void getArgs() override;

        NetAsyncOperation(JsValueRef callback, decltype(initFunc) initFunc, void* param)
            : JsScript::AsyncOperation(true, callback, initFunc, param)
            , status(0) {}
    };

private:
    virtual bool shouldInclude(std::wstring const& str) override {
        // deprecated name
        if (str == L"network") {
            return true;
        }

        return __super::shouldInclude(str);
    };

    static JsValueRef CALLBACK get(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                   unsigned short argCount, void* callbackState);

    static JsValueRef CALLBACK getSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                       unsigned short argCount, void* callbackState);


    static JsValueRef CALLBACK postSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                        unsigned short argCount, void* callbackState);
};

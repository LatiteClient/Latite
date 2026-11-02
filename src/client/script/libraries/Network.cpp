#include "pch.h"
#include "Network.h"

#include "util/HttpClient.h"

namespace {
http::Headers readHeaders(JsValueRef options) {
    http::Headers headers;
    auto headerArray = Chakra::GetProperty(options, L"headers");
    if (headerArray == JS_INVALID_REFERENCE) return headers;

    JsValueType type;
    JS::JsGetValueType(headerArray, &type);
    if (type != JsArray) return headers;

    int length = Chakra::GetIntProperty(headerArray, L"length");
    for (int index = 0; index < length; ++index) {
        JsValueRef item;
        JS::JsGetIndexedProperty(headerArray, Chakra::MakeInt(index), &item);
        JS::JsGetValueType(item, &type);
        if (type != JsObject) continue;

        headers.emplace_back(Chakra::GetStringProperty(item, L"name"),
                             Chakra::GetStringProperty(item, L"value"));
    }

    return headers;
}

JsValueRef makeResponse(http::Response const& response) {
    JsValueRef result;
    JS::JsCreateObject(&result);
    Chakra::SetPropertyNumber(result, L"statusCode", response.status, true);

    JsValueRef body;
    JS::JsCreateTypedArray(JsArrayTypeUint8, JS_INVALID_REFERENCE, 0,
                           static_cast<unsigned>(response.body.size()), &body);

    BYTE* storage;
    unsigned storageSize;
    JS::JsGetTypedArrayStorage(body, &storage, &storageSize, nullptr, nullptr);
    if (!response.body.empty()) {
        std::copy(response.body.begin(), response.body.end(), storage);
    }
    Chakra::SetProperty(result, L"body", body, true);

    return result;
}
}

JsValueRef Network::initialize(JsValueRef parent) {
    JsValueRef obj;
    JS::JsCreateObject(&obj);

    Chakra::DefineFunc(obj, get, L"getAsync", this);
    Chakra::DefineFunc(obj, getSync, L"get", this);
    Chakra::DefineFunc(obj, postSync, L"post", this);

    return obj;
}

JsValueRef Network::get(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                        unsigned short argCount, void* callbackState) {
    auto result = Chakra::GetUndefined();
    if (!Chakra::VerifyArgCount(argCount, 4)) return result;
    if (!Chakra::VerifyParameters(
            { { arguments[1], JsString }, { arguments[2], JsObject }, { arguments[3], JsFunction } }))
        return result;

    auto network = static_cast<Network*>(callbackState);
    auto operation = std::make_shared<NetAsyncOperation>(
        arguments[3],
        [](JsScript::AsyncOperation* base) {
            auto operation = static_cast<NetAsyncOperation*>(base);
            auto response = http::request(L"GET", operation->url, {}, operation->headers);

            if (response) {
                operation->status = response->status;
                operation->body = std::move(response->body);
            } else {
                operation->error = util::StrToWStr(response.error());
            }

            operation->flagDone = true;
        }, network);

    operation->url = Chakra::GetString(arguments[1]);
    operation->headers = readHeaders(arguments[2]);
    network->owner->pendingOperations.push_back(operation);
    operation->run();

    return result;
}

JsValueRef Network::getSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                            unsigned short argCount, void* callbackState) {
    auto result = Chakra::GetUndefined();
    if (!Chakra::VerifyArgCount(argCount, 2, true, true)) return result;
    if (!Chakra::VerifyParameters(
            { { arguments[1], JsString }, { Chakra::TryGet(arguments, argCount, 2), JsObject, true } }))
        return result;

    http::Headers headers;
    std::vector<std::uint8_t> body;
    auto options = Chakra::TryGet(arguments, argCount, 2);
    if (options != JS_INVALID_REFERENCE) {
        headers = readHeaders(options);
        auto content = util::WStrToStr(Chakra::GetStringProperty(options, L"content"));
        if (!content.empty()) {
            auto contentType = Chakra::GetStringProperty(options, L"contentType");
            if (contentType.empty()) contentType = L"text/plain";

            headers.emplace_back(L"Content-Type", contentType);
            body.assign(content.begin(), content.end());
        }
    }

    auto response = http::request(L"GET", Chakra::GetString(arguments[1]), body, headers);
    if (!response) {
        Chakra::ThrowError(util::StrToWStr(response.error()));
        return result;
    }

    return makeResponse(*response);
}

JsValueRef Network::postSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                             unsigned short argCount, void* callbackState) {
    auto result = Chakra::GetUndefined();
    if (!Chakra::VerifyArgCount(argCount, 3)) return result;
    if (!Chakra::VerifyParameters({ { arguments[1], JsString }, { arguments[2], JsObject } })) return result;

    auto options = arguments[2];
    auto headers = readHeaders(options);
    auto content = util::WStrToStr(Chakra::GetStringProperty(options, L"content"));
    auto contentType = Chakra::GetStringProperty(options, L"contentType");

    if (!content.empty()) {
        if (contentType.empty()) contentType = L"text/plain";
        headers.emplace_back(L"Content-Type", contentType);
    }

    std::vector<std::uint8_t> body(content.begin(), content.end());
    auto response = http::request(L"POST", Chakra::GetString(arguments[1]), body, headers);
    if (!response) {
        Chakra::ThrowError(util::StrToWStr(response.error()));
        return result;
    }

    return makeResponse(*response);
}

void Network::NetAsyncOperation::getArgs() {
    JsValueRef result;
    if (body) {
        http::Response response { static_cast<unsigned>(status), std::move(*body) };
        result = makeResponse(response);
    } else {
        JS::JsCreateObject(&result);
        Chakra::SetPropertyNumber(result, L"statusCode", status, true);
    }

    if (error) Chakra::SetPropertyString(result, L"error", *error);
    args.push_back(result);
}

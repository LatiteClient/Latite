#include "pch.h"
#include "JsWebSocket.h"

#include "client/Latite.h"
#include "util/HttpClient.h"

void WebSocketState::close() {
    HINTERNET handle = socket.exchange(nullptr);
    if (handle) {
        WinHttpCloseHandle(handle);
    }
}

WebSocketState::~WebSocketState() {
    close();
    if (connection) {
        WinHttpCloseHandle(connection);
    }
    if (session) {
        WinHttpCloseHandle(session);
    }
}

WebSocketHolder::WebSocketHolder(std::shared_ptr<WebSocketState> socketState, bool binaryMessages)
    : state(std::move(socketState)), binary(binaryMessages) {
    eventListeners[receiveEventId] = {};
    eventListeners[closeEventId] = {};
}

WebSocketHolder::~WebSocketHolder() {
    state->close();
}

std::shared_ptr<WebSocketHolder> JsWebSocketClass::getHolder(JsValueRef object) {
    std::shared_ptr<WebSocketHolder>* owner = nullptr;
    JS::JsGetExternalData(object, reinterpret_cast<void**>(&owner));
    if (!owner) return {};
    return *owner;
}

JsValueRef JsWebSocketClass::jsConstructor(JsValueRef callee, bool isConstructor,
                                           JsValueRef* arguments, unsigned short argCount,
                                           void* callbackState) {
    if (!Chakra::VerifyArgCount(argCount, 3)) return JS_INVALID_REFERENCE;
    if (!Chakra::VerifyParameters({ { arguments[1], JsString }, { arguments[2], JsNumber } })) {
        return JS_INVALID_REFERENCE;
    }

    int encoding = Chakra::GetInt(arguments[2]);
    if (encoding != 0 && encoding != 1) {
        Chakra::ThrowError(L"Unknown encoding");
        return JS_INVALID_REFERENCE;
    }

    std::wstring url = Chakra::GetString(arguments[1]);
    if (url.starts_with(L"wss://")) {
        url.replace(0, 6, L"https://");
    } else if (url.starts_with(L"ws://")) {
        url.replace(0, 5, L"http://");
    } else {
        Chakra::ThrowError(L"WebSocket URL must use ws:// or wss://");
        return JS_INVALID_REFERENCE;
    }

    URL_COMPONENTS parts {};
    parts.dwStructSize = sizeof(parts);
    parts.dwHostNameLength = static_cast<DWORD>(-1);
    parts.dwUrlPathLength = static_cast<DWORD>(-1);
    parts.dwExtraInfoLength = static_cast<DWORD>(-1);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) {
        Chakra::ThrowError(L"Invalid WebSocket URL");
        return JS_INVALID_REFERENCE;
    }

    std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    std::wstring path;
    if (parts.dwUrlPathLength) {
        path.assign(parts.lpszUrlPath, parts.dwUrlPathLength);
    }
    if (parts.dwExtraInfoLength) path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
    if (path.empty()) path = L"/";

    auto state = std::make_shared<WebSocketState>();
    state->session = WinHttpOpen(L"Latite Client/2.9", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                 WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!state->session) {
        Chakra::ThrowError(L"Could not open WebSocket session");
        return JS_INVALID_REFERENCE;
    }

    state->connection = WinHttpConnect(state->session, host.c_str(), parts.nPort, 0);
    if (!state->connection) {
        Chakra::ThrowError(L"Could not connect to WebSocket server");
        return JS_INVALID_REFERENCE;
    }

    DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET request = WinHttpOpenRequest(state->connection, L"GET", path.c_str(), nullptr,
                                            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!request) {
        Chakra::ThrowError(L"Could not create WebSocket request");
        return JS_INVALID_REFERENCE;
    }

    if (!WinHttpSetOption(request, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, nullptr, 0)) {
        WinHttpCloseHandle(request);
        Chakra::ThrowError(L"Could not request WebSocket upgrade");
        return JS_INVALID_REFERENCE;
    }

    if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        WinHttpCloseHandle(request);
        Chakra::ThrowError(L"Could not send WebSocket handshake");
        return JS_INVALID_REFERENCE;
    }

    if (!WinHttpReceiveResponse(request, nullptr)) {
        WinHttpCloseHandle(request);
        Chakra::ThrowError(L"Could not receive WebSocket handshake");
        return JS_INVALID_REFERENCE;
    }

    HINTERNET socket = WinHttpWebSocketCompleteUpgrade(request, 0);
    WinHttpCloseHandle(request);
    if (!socket) {
        Chakra::ThrowError(L"WebSocket upgrade failed");
        return JS_INVALID_REFERENCE;
    }

    state->socket = socket;
    auto holder = std::make_shared<WebSocketHolder>(state, encoding == 0);
    std::weak_ptr<WebSocketHolder> weakHolder = holder;
    JsContextRef context;
    JS::JsGetCurrentContext(&context);

    std::thread([state, weakHolder, socket, context] {
        std::array<std::uint8_t, 8192> buffer;
        std::vector<std::uint8_t> message;
        while (state->socket.load() == socket) {
            DWORD received = 0;
            WINHTTP_WEB_SOCKET_BUFFER_TYPE type;
            DWORD error = WinHttpWebSocketReceive(socket, buffer.data(),
                                                   static_cast<DWORD>(buffer.size()), &received, &type);
            if (error != ERROR_SUCCESS) break;
            if (type == WINHTTP_WEB_SOCKET_CLOSE_BUFFER_TYPE) {
                WinHttpWebSocketClose(socket, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, nullptr, 0);
                break;
            }

            bool finalChunk = type == WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE ||
                              type == WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE;

            message.insert(message.end(), buffer.data(), buffer.data() + received);
            if (!finalChunk) continue;

            Latite::get().queueForClientThread([weakHolder, context, bytes = std::move(message)] {
                auto holder = weakHolder.lock();
                if (!holder) return;

                Chakra::SetContext(context);
                JsValueRef payload;
                if (holder->binary) {
                    JS::JsCreateTypedArray(JsArrayTypeUint8, JS_INVALID_REFERENCE, 0,
                                           static_cast<unsigned>(bytes.size()), &payload);
                    BYTE* storage;
                    unsigned storageSize;
                    JS::JsGetTypedArrayStorage(payload, &storage, &storageSize, nullptr, nullptr);
                    if (!bytes.empty()) {
                        std::copy(bytes.begin(), bytes.end(), storage);
                    }
                } else {
                    std::string utf8(bytes.begin(), bytes.end());
                    payload = Chakra::MakeString(util::StrToWStr(utf8));
                }

                JsEvented::Event event(std::wstring(WebSocketHolder::receiveEventId), { payload });
                holder->dispatchEvent(event);
            });

            message.clear();
        }

        state->close();
        Latite::get().queueForClientThread([weakHolder] {
            auto holder = weakHolder.lock();
            if (!holder) return;

            JsEvented::Event event(std::wstring(WebSocketHolder::closeEventId), {});
            holder->dispatchEvent(event);
        });
    }).detach();

    auto client = static_cast<JsWebSocketClass*>(callbackState);
    return client->construct(holder);
}

JsValueRef JsWebSocketClass::send(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                  unsigned short argCount, void* callbackState) {
    if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;

    auto holder = getHolder(arguments[0]);
    if (!holder) {
        Chakra::ThrowError(L"Object is not a WebSocket");
        return JS_INVALID_REFERENCE;
    }

    JsValueType type;
    JS::JsGetValueType(arguments[1], &type);

    std::vector<std::uint8_t> bytes;
    if (type == JsString) {
        auto utf8 = util::WStrToStr(Chakra::GetString(arguments[1]));
        bytes.assign(utf8.begin(), utf8.end());
    } else if (type == JsTypedArray) {
        BYTE* storage;
        unsigned size;
        JsTypedArrayType arrayType;
        JS::JsGetTypedArrayStorage(arguments[1], &storage, &size, &arrayType, nullptr);
        if (arrayType != JsArrayTypeUint8) {
            Chakra::ThrowError(L"Expected Uint8Array for argument 1");
            return JS_INVALID_REFERENCE;
        }

        bytes.assign(storage, storage + size);
    } else {
        Chakra::ThrowError(L"Expected Uint8Array or string for argument 1");
        return JS_INVALID_REFERENCE;
    }

    auto bufferType = holder->binary ? WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE
                                     : WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE;
    HINTERNET socket = holder->state->socket.load();
    if (!socket) {
        Chakra::ThrowError(L"WebSocket is closed");
        return JS_INVALID_REFERENCE;
    }

    DWORD error = WinHttpWebSocketSend(socket, bufferType,
                                       bytes.empty() ? nullptr : bytes.data(),
                                       static_cast<DWORD>(bytes.size()));
    if (error != ERROR_SUCCESS) {
        Chakra::ThrowError(util::StrToWStr(http::errorMessage("WinHttpWebSocketSend", error)));
        return JS_INVALID_REFERENCE;
    }

    return Chakra::GetUndefined();
}

JsValueRef JsWebSocketClass::close(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                   unsigned short argCount, void* callbackState) {
    auto holder = getHolder(arguments[0]);
    if (!holder) {
        Chakra::ThrowError(L"Object is not a WebSocket");
        return JS_INVALID_REFERENCE;
    }

    int code = 1000;
    std::wstring reason;
    if (argCount > 1) {
        JsValueType type;
        JS::JsGetValueType(arguments[1], &type);
        if (type != JsNumber) {
            Chakra::ThrowError(L"Expected number for argument 1");
            return JS_INVALID_REFERENCE;
        }

        code = Chakra::GetInt(arguments[1]);
    }

    if (argCount > 2) {
        JsValueType type;
        JS::JsGetValueType(arguments[2], &type);
        if (type != JsString) {
            Chakra::ThrowError(L"Expected string for argument 2");
            return JS_INVALID_REFERENCE;
        }

        reason = Chakra::GetString(arguments[2]);
    }

    auto utf8Reason = util::WStrToStr(reason);
    if (code < 0 || code > 65535 || utf8Reason.size() > WINHTTP_WEB_SOCKET_MAX_CLOSE_REASON_LENGTH) {
        Chakra::ThrowError(L"Invalid WebSocket close code or reason");
        return JS_INVALID_REFERENCE;
    }

    HINTERNET socket = holder->state->socket.load();
    if (socket) {
        WinHttpWebSocketShutdown(socket, static_cast<USHORT>(code),
                                  utf8Reason.empty() ? nullptr : utf8Reason.data(),
                                  static_cast<DWORD>(utf8Reason.size()));
        holder->state->close();
    }

    return Chakra::GetUndefined();
}

JsValueRef JsWebSocketClass::construct(std::shared_ptr<WebSocketHolder> holder) {
    JsValueRef object;
    auto externalData = new std::shared_ptr<WebSocketHolder>(std::move(holder));
    JS::JsCreateExternalObject(externalData, [](void* data) {
        delete static_cast<std::shared_ptr<WebSocketHolder>*>(data);
    }, &object);
    JS::JsSetPrototype(object, getPrototype());
    return object;
}

JsValueRef JsWebSocketClass::on(JsValueRef callee, bool isConstructor, JsValueRef* arguments,
                                unsigned short argCount, void* callbackState) {
    if (!Chakra::VerifyArgCount(argCount, 3)) return Chakra::GetUndefined();
    if (!Chakra::VerifyParameters({ { arguments[1], JsString }, { arguments[2], JsFunction } })) {
        return JS_INVALID_REFERENCE;
    }

    auto holder = getHolder(arguments[0]);
    if (!holder) {
        Chakra::ThrowError(L"Object is not a WebSocket");
        return JS_INVALID_REFERENCE;
    }

    auto eventName = Chakra::GetString(arguments[1]);
    auto listener = holder->eventListeners.find(eventName);
    if (listener == holder->eventListeners.end()) {
        Chakra::ThrowError(L"Unknown event " + eventName);
        return JS_INVALID_REFERENCE;
    }

    JsContextRef context;
    JS::JsGetCurrentContext(&context);
    JS::JsAddRef(arguments[2], nullptr);
    listener->second.emplace_back(arguments[2], context);
    return arguments[0];
}

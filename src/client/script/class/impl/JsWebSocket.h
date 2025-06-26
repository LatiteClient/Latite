#pragma once
#include "../JsWrapperClass.h"
#include "../../interop/classes/JsTexture.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace std::string_view_literals;

using winrt::Windows::Networking::Sockets::MessageWebSocket;
using winrt::Windows::Storage::Streams::DataWriter;

struct WebSocketHolder : public JsEvented {
	inline static std::wstring_view receiveEventId = L"receive"sv;
	inline static std::wstring_view closeEventId = L"close"sv;

	MessageWebSocket socket;
	DataWriter writer{ nullptr };

	WebSocketHolder(MessageWebSocket&& ptr) : socket(ptr) {
		this->eventListeners[receiveEventId] = {};
		this->eventListeners[closeEventId] = {};
		writer = DataWriter(socket.OutputStream());
	}
};

class JsWebSocketClass final : public JsWrapperClass<WebSocketHolder> {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK send(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK close(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK on(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		return Chakra::MakeString(L"[object " + std::wstring(reinterpret_cast<JsWebSocketClass*>(callbackState)->name) + L"]");
	}
public:
	inline static const wchar_t* class_name = L"WebSocket";

	JsWebSocketClass(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	virtual JsValueRef construct(WebSocketHolder*, bool del);

	void prepareFunctions() override {
		// Static functions
		// Member functions
		Chakra::DefineFunc(prototype, send, XW("send"), this);
		Chakra::DefineFunc(prototype, close, XW("close"), this);
		Chakra::DefineFunc(prototype, on, XW("on"), this);
		Chakra::DefineFunc(prototype, toStringCallback, XW("toString"), this);
	};
private:
	winrt::Windows::Networking::Sockets::MessageWebSocket webSocket;
};
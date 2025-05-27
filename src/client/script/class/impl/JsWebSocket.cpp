#include "pch.h"
#include "JsWebSocket.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

// (url: string, encoding: int) -> WebSocketHolder
JsValueRef JsWebSocketClass::jsConstructor(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsNumber} })) return JS_INVALID_REFERENCE;

	auto this_ = reinterpret_cast<JsWebSocketClass*>(callbackState);

	auto holder = new WebSocketHolder(MessageWebSocket{});
	auto& socket = holder->socket;

	auto url = Chakra::GetString(arguments[1]);
	auto encoding = Chakra::GetInt(arguments[2]);
	
	if (encoding == 0) {
		socket.Control().MessageType(SocketMessageType::Binary);
	}
	else if (encoding == 1) {
		socket.Control().MessageType(SocketMessageType::Utf8);
	}
	else {
		Chakra::ThrowError(L"Unknown encoding");
		return JS_INVALID_REFERENCE;
	}

	// fire JS message received event
	socket.MessageReceived([holder](const MessageWebSocket& socket, const MessageWebSocketMessageReceivedEventArgs& args) {
		DataReader reader = args.GetDataReader();
		reader.UnicodeEncoding(UnicodeEncoding::Utf8);

		if (socket.Control().MessageType() == SocketMessageType::Utf8) {
			// create a string with the received data

			winrt::hstring message = reader.ReadString(reader.UnconsumedBufferLength());
			Latite::get().queueForClientThread([=] {
				JsEvented::Event ev{ std::wstring(WebSocketHolder::receiveEventId), {Chakra::MakeString(std::wstring(message)) } };
				holder->dispatchEvent(ev);
				});

			
		}
		else if (socket.Control().MessageType() == SocketMessageType::Binary) {
			// create a Uint8Array with the received data

			uint32_t size = reader.UnconsumedBufferLength();

			auto buf = reader.ReadBuffer(reader.UnconsumedBufferLength());

			std::vector<uint8_t> bytes;
			bytes.reserve(size);

			for (size_t i = 0; i < buf.Length(); ++i) {
				bytes[i] = buf.data()[i];
			}

			Latite::get().queueForClientThread([=] {
				JsValueRef array;
				JS::JsCreateTypedArray(JsArrayTypeUint8, nullptr, 0, static_cast<unsigned int>(bytes.size()), &array);

				BYTE* chakraData;
				JS::JsGetTypedArrayStorage(array, &chakraData, nullptr, nullptr, nullptr);

				for (size_t i = 0; i < bytes.size(); ++i) {
					chakraData[i] = bytes[i];
				}

				JsEvented::Event ev{ std::wstring(WebSocketHolder::receiveEventId), {array} };
				holder->dispatchEvent(ev);
				});
		}
		});

	// fire JS websocket closed event
	socket.Closed([holder](const IWebSocket& socket, const WebSocketClosedEventArgs& args) {
		Latite::get().queueForClientThread([=] {
			JsEvented::Event ev{ std::wstring(WebSocketHolder::closeEventId), {} };
			holder->dispatchEvent(ev);
			});

		});

	try {
		socket.ConnectAsync(Uri(url)).get();
	}
	catch (winrt::hresult_error&) {
		Chakra::ThrowError(L"Unable to connect to websocket server");
		return JS_INVALID_REFERENCE;
	}

	return this_->construct(holder, true);
}

// (data: string | Uint8Array) -> void
JsValueRef JsWebSocketClass::send(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	
	JsValueType type;
	JS::JsGetValueType(arguments[1], &type);
	
	auto holder = Get(arguments[0]);
	
	DataWriter writer(holder->socket.OutputStream());
	if (type == JsString) {
		writer.UnicodeEncoding(UnicodeEncoding::Utf8);
		writer.WriteString(Chakra::GetString(arguments[1]));
	}
	else if (type == JsTypedArray) {
		JsTypedArrayType arrayType;
		uint32_t size;

		BYTE* chakraData;
		JS::JsGetTypedArrayStorage(arguments[1], &chakraData, &size, &arrayType, nullptr);

		// make sure it is actually a Uint8Array, not a Uint16/32Array
		if (arrayType != JsArrayTypeUint8) {
			Chakra::ThrowError(L"Expected Uint8Array for argument 1");
			return JS_INVALID_REFERENCE;
		}

		writer.WriteBytes({ chakraData, size });
	}
	else {
		Chakra::ThrowError(L"Expected Uint8Array or string for argument 1");
		return JS_INVALID_REFERENCE;
	}

	writer.StoreAsync().get();
	writer.FlushAsync().get();

	return Chakra::GetUndefined();
}

// (code?: int, reason?: string) -> void
JsValueRef JsWebSocketClass::close(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
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
	
	auto holder = Get(arguments[0]);
	holder->socket.Close(static_cast<uint16_t>(code), reason);

	return Chakra::GetUndefined();
}

JsValueRef JsWebSocketClass::construct(WebSocketHolder* ptr, bool del) {
	JsValueRef obj;
	if (del) {
		JS::JsCreateExternalObject(ptr, [](void* obj) {
			delete obj;
			}, &obj);
	}
	else {
		JS::JsCreateExternalObject(ptr, [](void*) {
			}, &obj);
	}
	JS::JsSetPrototype(obj, getPrototype());
	return obj;
}

JsValueRef JsWebSocketClass::on(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3)) return Chakra::GetUndefined();

	if (!Chakra::VerifyParameters({ {arguments[1], JsString }, {arguments[2], JsFunction} })) return JS_INVALID_REFERENCE;

	auto holder = Get(arguments[0]);
	if (!holder) {
		Chakra::ThrowError(L"Object is not a WebSocket");
		return Chakra::GetUndefined();
	}

	auto str = Chakra::GetString(arguments[1]);

	if (holder->eventListeners.find(str) != holder->eventListeners.end()) {
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsAddRef(arguments[2], nullptr);
		holder->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
		return arguments[0];
	}
	Chakra::ThrowError(L"Unknown event " + str);
	return Chakra::GetUndefined();
}

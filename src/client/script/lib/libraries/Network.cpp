/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "Network.h"

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/impl/windows.web.http.2.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/windows.web.http.headers.h>

using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

JsValueRef Network::initialize(JsValueRef parent) {
    JsValueRef obj;
    JS::JsCreateObject(&obj);

	Chakra::DefineFunc(obj, get, L"get", this);
	Chakra::DefineFunc(obj, getSync, L"getSync", this);
	Chakra::DefineFunc(obj, postSync, L"postSync", this);

    return obj;
}

JsValueRef Network::get(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 4)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsObject}, {arguments[3], JsFunction}})) return ret;
	auto ws = Chakra::GetString(arguments[1]);
	
	auto thi = reinterpret_cast<Network*>(callbackState);
	auto op = std::make_shared<NetAsyncOperation>(arguments[3], [](JsScript::AsyncOperation* op_) {
		auto op = reinterpret_cast<NetAsyncOperation*>(op_);
		auto http = HttpClient();

		try {
			winrt::Windows::Foundation::Uri requestUri(op->url);

			HttpRequestMessage request(HttpMethod::Get(), requestUri);

			auto operation = http.SendRequestAsync(request);
			auto response = operation.get();

			op->err = (int)response.StatusCode();

			std::wstring strs;
			auto cont = response.Content();
			auto buffer = cont.ReadAsBufferAsync().get();

			op->data = buffer;
		}
		catch (winrt::hresult_error const& err) {
			op->winrtErr = err.message();
		}
		op->flagDone = true;
		}, thi);

	op->url = Chakra::GetString(arguments[1]);

	op->run();
	
	thi->owner->pendingOperations.push_back(op);
	return ret;
}

JsValueRef Network::getSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {Chakra::TryGet(arguments, argCount, 2), JsObject, true}} )) return ret;
	auto ws = Chakra::GetString(arguments[1]);

	auto http = HttpClient();

	winrt::Windows::Foundation::Uri requestUri(ws);

	HttpRequestMessage request(HttpMethod::Get(), requestUri);

	auto arg2 = Chakra::TryGet(arguments, argCount, 2);

	if (arg2 != JS_INVALID_REFERENCE) {
		auto cont = Chakra::GetStringProperty(arg2, L"content");
		if (!cont.empty()) {

			auto content = HttpStringContent(cont);
			// obj
			auto contTyp = Chakra::GetStringProperty(arg2, L"contentType");
			if (contTyp.empty()) {
				contTyp = L"text/plain";
				content.Headers().ContentType().MediaType(contTyp);
			}

			request.Content(content);
		}
	}

	try {
		auto operation = http.SendRequestAsync(request);
		auto response = operation.get();

		auto cont = response.Content();
		auto buffer = cont.ReadAsBufferAsync().get();
		JS::JsCreateObject(&ret);
		Chakra::SetPropertyNumber(ret, L"statusCode", static_cast<double>(response.StatusCode()), true);

		JsValueRef jsBuffer;
		JS::JsCreateTypedArray(JsArrayTypeUint8, JS_INVALID_REFERENCE, 0, buffer.Length(), &jsBuffer);
		BYTE* bytes;
		unsigned int byteCount;
		JsTypedArrayType arrayType;
		int elementSize;
		JS::JsGetTypedArrayStorage(jsBuffer, &bytes, &byteCount, &arrayType, &elementSize);

		auto bufData = buffer.data();
		for (unsigned int i = 0; i < buffer.Length(); ++i) {
			bytes[i] = bufData[i];
		}

		Chakra::SetProperty(ret, L"body", jsBuffer);
	}
	catch (winrt::hresult_error const& err) {
		Chakra::ThrowError(std::wstring(err.message().c_str()));
	}
	return ret;
}

JsValueRef Network::post(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return JsValueRef();
}

JsValueRef Network::postSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsObject}})) return ret;
	auto ws = Chakra::GetString(arguments[1]);

	auto http = HttpClient();

	winrt::Windows::Foundation::Uri requestUri(ws);

	HttpRequestMessage request(HttpMethod::Post(), requestUri);

	auto arg2 = arguments[2];

	if (arg2 != JS_INVALID_REFERENCE) {
		auto cont = Chakra::GetStringProperty(arg2, L"content");
		if (!cont.empty()) {

			auto content = HttpStringContent(cont);
			// obj
			auto contTyp = Chakra::GetStringProperty(arg2, L"contentType");
			if (contTyp.empty()) {
				contTyp = L"text/plain";
				content.Headers().ContentType().MediaType(contTyp);
			}

			request.Content(content);
		}
	}

	try {
		auto operation = http.SendRequestAsync(request);
		auto response = operation.get();

		auto cont = response.Content();
		auto buffer = cont.ReadAsBufferAsync().get();

		JS::JsCreateObject(&ret);
		Chakra::SetPropertyNumber(ret, L"statusCode", static_cast<double>(response.StatusCode()), true);

		JsValueRef jsBuffer;
		JS::JsCreateTypedArray(JsArrayTypeUint8, JS_INVALID_REFERENCE, 0, buffer.Length(), &jsBuffer);
		BYTE* bytes;
		unsigned int byteCount;
		JsTypedArrayType arrayType;
		int elementSize;
		JS::JsGetTypedArrayStorage(jsBuffer, &bytes, &byteCount, &arrayType, &elementSize);

		auto bufData = buffer.data();
		for (unsigned int i = 0; i < buffer.Length(); ++i) {
			bytes[i] = bufData[i];
		}

		Chakra::SetPropertyNumber(ret, L"statusCode", static_cast<double>(response.StatusCode()), true);
		Chakra::SetProperty(ret, L"body", jsBuffer);
	}
	catch (winrt::hresult_error const& err) {
		Chakra::ThrowError(std::wstring(err.message()));
	}
	return ret;
}

void Network::NetAsyncOperation::getArgs() {
	JsValueRef obj;
	JS::JsCreateObject(&obj);

	Chakra::SetPropertyNumber(obj, L"statusCode", (double)this->err, true);

	if (data.has_value()) {
		//JS::JsPointerToString(this->data.value().c_str(), this->data.value().size(), &data);
		JsValueRef jsBuffer;
		JS::JsCreateTypedArray(JsArrayTypeUint8, JS_INVALID_REFERENCE, 0, this->data->Length(), &jsBuffer);
		BYTE* bytes;
		unsigned int byteCount;
		JsTypedArrayType arrayType;
		int elementSize;
		JS::JsGetTypedArrayStorage(jsBuffer, &bytes, &byteCount, &arrayType, &elementSize);

		auto bufData = this->data->data();
		for (unsigned int i = 0; i < this->data->Length(); ++i) {
			bytes[i] = bufData[i];
		}

		Chakra::SetProperty(obj, L"body", jsBuffer, true);
	}

	if (winrtErr.has_value()) {
		Chakra::SetPropertyString(obj, L"error", winrtErr.value());
	}

	this->args.push_back(obj);
}

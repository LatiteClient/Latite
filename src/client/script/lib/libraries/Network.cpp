#include "Network.h"

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/impl/windows.web.http.2.h>
#include <winrt/Windows.Web.Http.Filters.h>

using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

JsValueRef Network::initialize(JsValueRef parent) {
    JsValueRef obj;
    JS::JsCreateObject(&obj);

	Chakra::DefineFunc(obj, get, L"get", this);
	Chakra::DefineFunc(obj, getSync, L"getSync", this);

    return obj;
}

JsValueRef Network::get(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsFunction} })) return ret;
	auto ws = Chakra::GetString(arguments[1]);
	
	auto thi = reinterpret_cast<Network*>(callbackState);
	auto op = std::make_shared<NetAsyncOperation>(arguments[2], [](JsScript::AsyncOperation* op_) {
		auto op = reinterpret_cast<NetAsyncOperation*>(op_);
		auto http = HttpClient();

		winrt::Windows::Foundation::Uri requestUri(op->url);

		HttpRequestMessage request(HttpMethod::Get(), requestUri);

		auto operation = http.SendRequestAsync(request);
		auto response = operation.get();

		op->err = (int)response.StatusCode();

		std::wstring strs;
		if (op->err == 200) {
			auto cont = response.Content();
			strs = cont.ReadAsStringAsync().get();
		}
		if (op->err == 200) op->data = strs;
		op->flagDone = true;
		}, thi);

	op->url = Chakra::GetString(arguments[1]);

	op->run();
	
	thi->owner->pendingOperations.push_back(op);
	return ret;
}

JsValueRef Network::getSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;
	auto ws = Chakra::GetString(arguments[1]);

	auto http = HttpClient();

	winrt::Windows::Foundation::Uri requestUri(ws);

	HttpRequestMessage request(HttpMethod::Get(), requestUri);

	try {
		auto operation = http.SendRequestAsync(request);
		auto response = operation.get();

		auto cont = response.Content();
		auto strs = cont.ReadAsStringAsync().get();
		JS::JsCreateObject(&ret);
		Chakra::SetPropertyNumber(ret, L"statusCode", static_cast<double>(response.StatusCode()), true);
		Chakra::SetPropertyString(ret, L"body", strs.c_str(), true);
	}
	catch (winrt::hresult_error const& err) {
		Chakra::ThrowError(std::wstring(err.message().c_str()));
	}
	return ret;
}

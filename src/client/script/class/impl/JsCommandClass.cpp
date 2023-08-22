#include "JsCommandClass.h"

JsValueRef JsCommandClass::jsConstructor(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto thi = reinterpret_cast<JsCommandClass*>(callbackState);

	if (!isConstructor) return thi->errConstructCall();

	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsArray} })) return JS_INVALID_REFERENCE;

	auto array = arguments[4];
	std::vector<std::string> aliases = {};

	auto len = Chakra::GetProperty(array, L"length");
	for (int i = 0; i < Chakra::GetInt(len); ++i) {
		JsValueRef str;
		JS::JsGetIndexedProperty(array, Chakra::MakeInt(i), &str);
		aliases.push_back(util::WStrToStr(Chakra::GetString(str)));
		JS::JsRelease(str, nullptr);
	}

	auto jsc = new JsCommand(util::WStrToStr(Chakra::GetString(arguments[1])), util::WStrToStr(Chakra::GetString(arguments[2])), util::WStrToStr(Chakra::GetString(arguments[3])), aliases);
	return thi->construct(jsc, true);
}

JsValueRef JsCommandClass::setOnEvent(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3)) return Chakra::GetUndefined();

	if (!Chakra::VerifyParameters({ {arguments[1], JsString }, {arguments[2], JsFunction} })) return JS_INVALID_REFERENCE;

	JsCommand* cmd;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&cmd));
	if (!cmd) {
		Chakra::ThrowError(L"Object is not a command");
		return Chakra::GetUndefined();
	}
	auto str = Chakra::GetString(arguments[1]);
	if (cmd->eventListeners.find(str) != cmd->eventListeners.end()) {
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsAddRef(arguments[2], nullptr);
		cmd->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
		return Chakra::GetUndefined();
	}
	Chakra::ThrowError(L"Unknown event " + str);
	return Chakra::GetUndefined();
}
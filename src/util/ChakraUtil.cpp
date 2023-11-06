#include "pch.h"
#include "ChakraUtil.h"
#include "Util.h"
#include "client/Latite.h"
#include "Logger.h"
#include <filesystem>

#include <winrt/Windows.Web.Http.h>
#include <winrt/windows.storage.h>
#include <winrt/windows.storage.streams.h>

using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage;

FARPROC Chakra::pass(const char* name)
{
	if (!mod) {
		mod = GetModuleHandleA("Chakra.dll");

		// https://raw.githubusercontent.com/Imrglop/Latite-Releases/main/bin/ChakraCore.dll

		//mod = LoadLibraryW(L"C:\\Windows\\system32\\Chakra.dll");
		// sadly going to have to cope with chakra and not chakracore

		auto latitePath = util::GetLatitePath();
		std::filesystem::create_directory(latitePath);
		auto assetsPath = latitePath / "Assets";
		std::filesystem::create_directory(latitePath / "Assets");

		if (!std::filesystem::exists(assetsPath / "ChakraCore.dll")) {
			Logger::Info("ChakraCore.dll not found, downloading..");
			Latite::get().downloadChakraCore();
		}

		if (!mod) mod = LoadLibraryW((util::GetLatitePath() / "Assets" / "ChakraCore.dll").wstring().c_str());
	}
	if (!mod) {
		return 0;
	}
	return GetProcAddress(mod, name);
}

void Chakra::SetProperty(JsValueRef ref, std::wstring name, JsValueRef value, bool strict) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);
	JS::JsSetProperty(ref, propId, value, strict);
}

void Chakra::SetPropertyString(JsValueRef ref, std::wstring name, std::wstring value, bool strict) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef strRef;
	JS::JsPointerToString(value.c_str(), value.size(), &strRef);
	JS::JsSetProperty(ref, propId, strRef, strict);

	JS::JsRelease(strRef, nullptr);
}

void Chakra::SetPropertyNumber(JsValueRef ref, std::wstring name, double value, bool strict) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef num;
	JS::JsDoubleToNumber(value, &num);
	JS::JsSetProperty(ref, propId, num, strict);

	JS::JsRelease(num, nullptr);
}

void Chakra::SetPropertyBool(JsValueRef ref, std::wstring name, bool value, bool strict) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef num;
	JS::JsBoolToBoolean(value, &num);
	JS::JsSetProperty(ref, propId, num, strict);

	JS::JsRelease(num, nullptr);
}

void Chakra::SetPropertyObject(JsValueRef ref, std::wstring name, JsValueRef obj, bool strict)
{
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);
	JS::JsSetProperty(ref, propId, obj, strict);
}

unsigned int Chakra::GetRefCount(JsValueRef obj) {
	unsigned count;
	JS::JsAddRef(obj, &count);
	JS::JsRelease(obj, &count);
	return count;
}

JsErrorCode Chakra::CallFunction(JsValueRef func, JsValueRef* arguments, unsigned short argumentCount, JsValueRef* result) {
	return JS::JsCallFunction(func, arguments, argumentCount, result);
}


JsValueRef Chakra::GetProperty(JsValueRef obj, std::wstring name) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);
	JsValueRef ret;
	JS::JsGetProperty(obj, propId, &ret);
	return ret;
}

JsValueRef Chakra::TryGet(JsValueRef* args, unsigned short count, unsigned short idx) {
	if (idx >= count) return JS_INVALID_REFERENCE;
	return args[idx];
}

void Chakra::DefineFunc(JsValueRef obj, JsNativeFunction func, std::wstring name, void* state) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef vf;
	JS::JsCreateFunction(func, state, &vf);
	JS::JsSetProperty(obj, propId, vf, true);

	JS::JsRelease(vf, nullptr);
}


std::wstring Chakra::GetStringProperty(JsValueRef ref, std::wstring name) {
	JsPropertyIdRef nameId;
	JS::JsGetPropertyIdFromName(name.c_str(), &nameId);

	JsValueRef val;
	auto err = JS::JsGetProperty(ref, nameId, &val);
	if (err != JsNoError) {
		return L"";
	}

	const wchar_t* str;
	size_t len;
	err = JS::JsStringToPointer(val, &str, &len);

	if (err != JsNoError) {
		Release(val);
		return L"";
	}

	Release(val);
	return std::wstring(str);
}

bool Chakra::GetBoolProperty(JsValueRef ref, std::wstring name) {
	JsPropertyIdRef nameId;
	JS::JsGetPropertyIdFromName(name.c_str(), &nameId);

	JsValueRef val;
	JS::JsGetProperty(ref, nameId, &val);

	bool b;
	JS::JsBooleanToBool(val, &b);

	Release(val);
	return b;
}

double Chakra::GetNumberProperty(JsValueRef ref, std::wstring name) {
	JsPropertyIdRef nameId;
	JS::JsGetPropertyIdFromName(name.c_str(), &nameId);

	JsValueRef val;
	JS::JsGetProperty(ref, nameId, &val);

	double b;
	JS::JsNumberToDouble(val, &b);
	Release(val);
	return b;
}

std::wstring Chakra::ToString(JsValueRef ref) {
	JsValueRef r2;
	JS::JsConvertValueToString(ref, &r2);
	const wchar_t* str;
	size_t len;
	JS::JsStringToPointer(r2, &str, &len);
	std::wstring wstr(str);
	Release(r2);
	return wstr;
}

double Chakra::GetNumber(JsValueRef ref) {
	double db;
	JS::JsNumberToDouble(ref, &db);
	return db;
}

int Chakra::GetIntProperty(JsValueRef ref, std::wstring name) {
	JsPropertyIdRef nameId;
	JS::JsGetPropertyIdFromName(name.c_str(), &nameId);

	JsValueRef val;
	JS::JsGetProperty(ref, nameId, &val);

	int b;
	JS::JsNumberToInt(val, &b);
	Release(val);
	return b;
}

int Chakra::GetInt(JsValueRef ref) {
	int num;
	JS::JsNumberToInt(ref, &num);
	return num;
}

bool Chakra::GetBool(JsValueRef ref) {
	bool b;
	JS::JsBooleanToBool(ref, &b);
	return b;
}

JsValueRef Chakra::GetNull() {
	JsValueRef ret;
	JS::JsGetNullValue(&ret);
	return ret;
}

void Chakra::HandleErrors(JsErrorCode err) {
	if (err == JsErrorScriptException) {

	}
}

d2d::Rect Chakra::GetRectFromJs(JsValueRef obj) {
	return { static_cast<float>(Chakra::GetNumberProperty(obj, L"left")), static_cast<float>(Chakra::GetNumberProperty(obj, L"top")),
	static_cast<float>(Chakra::GetNumberProperty(obj, L"right")), static_cast<float>(Chakra::GetNumberProperty(obj, L"bottom")) };
}

Vec2 Chakra::GetVec2FromJs(JsValueRef obj) {
	return { static_cast<float>(Chakra::GetNumberProperty(obj, L"x")), static_cast<float>(Chakra::GetNumberProperty(obj, L"y")) };
}

Vec3 Chakra::GetVec3FromJs(JsValueRef obj) {
	return { static_cast<float>(Chakra::GetNumberProperty(obj, L"x")), static_cast<float>(Chakra::GetNumberProperty(obj, L"y")), static_cast<float>(Chakra::GetNumberProperty(obj, L"z")) };
}

d2d::Color Chakra::GetColorFromJs(JsValueRef obj) {
	d2d::Color col;

	JsValueRef zero;
	JsValueRef one;
	JsValueRef two;
	JsValueRef three;

	JS::JsIntToNumber(0, &zero);
	JS::JsIntToNumber(1, &one);
	JS::JsIntToNumber(2, &two);
	JS::JsIntToNumber(3, &three);

	JsValueRef z;
	JsValueRef o;
	JsValueRef t;
	JsValueRef th;

	JS::JsGetIndexedProperty(obj, zero, &z);
	JS::JsGetIndexedProperty(obj, one, &o);
	JS::JsGetIndexedProperty(obj, two, &t);
	JS::JsGetIndexedProperty(obj, three, &th);

	col.r = static_cast<float>(Chakra::GetNumber(z));
	col.g = static_cast<float>(Chakra::GetNumber(o));
	col.b = static_cast<float>(Chakra::GetNumber(t));
	col.a = static_cast<float>(Chakra::GetNumber(th));


	Chakra::Release(zero);
	Chakra::Release(one);
	Chakra::Release(two);
	Chakra::Release(three);

	Chakra::Release(z);
	Chakra::Release(o);
	Chakra::Release(t);
	Chakra::Release(th);
	return col;
}

Chakra::Result Chakra::VerifyParameters(std::initializer_list<ParamContainer> params, bool autoThrow) {
	size_t count = 0;
	for (auto const& param : params) {
		count++;
		if (param.isOptional && param.val == JS_INVALID_REFERENCE) continue;
		JsValueType trueType;
		JS::JsGetValueType(param.val, &trueType);
		if (param.isOptional && (param.val == JS_INVALID_REFERENCE || trueType == JsUndefined || trueType == JsNull)) continue;


		if (trueType != param.type) {
			std::wstring wstr = L"Parameter " + std::to_wstring(count) + L" must be of correct type (has " + Chakra::GetTypeName(trueType) + L", needs " + Chakra::GetTypeName(param.type) + L")";
			if (autoThrow) {
				ThrowError(wstr);
			}

			return { false,wstr };
		}
	}
	return { true, L"" };
}

std::wstring Chakra::GetString(JsValueRef ref) {
	const wchar_t* str;
	size_t sz;
	JS::JsStringToPointer(ref, &str, &sz);
	return std::wstring(str);
}

void Chakra::ThrowError(std::wstring message) {
	JsValueRef errorValue;
	JsValueRef errorMessageValue;

	JS::JsPointerToString(message.c_str(), message.size(), &errorMessageValue);
	JS::JsCreateError(errorMessageValue, &errorValue);
	JS::JsSetException(errorValue);
}

void Chakra::Release(JsValueRef ref) {
	JS::JsRelease(ref, nullptr);
}

JsValueRef Chakra::GetUndefined() {
	JsValueRef ret;
	JS::JsGetUndefinedValue(&ret);
	return ret;
}

Chakra::Result Chakra::VerifyArgCount(unsigned short has, unsigned short expected, bool autoThrow, bool least) {
	if (has != expected || (least && has > expected)) {
		auto ws = L"Function does not take " + std::to_wstring(has - 1) + L" arguments";
		if (autoThrow) Chakra::ThrowError(ws);
		return { false, ws };
	}
	return { true, L"" };
}

JsValueRef Chakra::GetTrue() {
	JsValueRef ret;
	JS::JsGetTrueValue(&ret);
	return ret;
}

JsValueRef Chakra::GetFalse() {
	JsValueRef ret;
	JS::JsGetFalseValue(&ret);
	return ret;
}

std::wstring Chakra::GetTypeName(JsValueType type) {
	switch (type) {
	case JsValueType::JsUndefined:
		return L"undefined";
	case JsValueType::JsNull:
		return L"null";
	case JsValueType::JsNumber:
		return L"number";
	case JsValueType::JsString:
		return L"string";
	case JsValueType::JsObject:
		return L"object";
	case JsValueType::JsFunction:
		return L"function";
	case JsValueType::JsArray:
		return L"array";
	case JsValueType::JsError:
		return L"error";
	case JsValueType::JsBoolean:
		return L"boolean";
	case JsValueType::JsSymbol:
		return L"symbol";
	case JsValueType::JsTypedArray:
		return L"typed array";
	default:
		return L"<unknown type>";
	}
}

JsValueRef Chakra::MakeString(std::wstring const& ws) {
	JsValueRef str;
	JS::JsPointerToString(ws.c_str(), ws.size(), &str);
	return str;
}

JsValueRef Chakra::MakeInt(int num) {
	JsValueRef val;
	JS::JsIntToNumber(num, &val);
	return val;
}

JsValueRef Chakra::MakeDouble(double num) {
	JsValueRef val;
	JS::JsDoubleToNumber(num, &val);
	return val;
}

#pragma once
#include "../JsWrapperClass.h"
#include "util/LMath.h"
#include "util/ChakraUtil.h"
#include <array>

static std::array<FARPROC, 16> banList = {
			(FARPROC)VirtualProtect,
			(FARPROC)VirtualProtectEx,
			(FARPROC)mouse_event,
			(FARPROC)keybd_event,
			(FARPROC)SendInput,
			(FARPROC)CreateThread,
			(FARPROC)OpenProcess,
			(FARPROC)OpenProcessToken,
			(FARPROC)LoadLibraryA,
			(FARPROC)LoadLibraryW,
			(FARPROC)LoadLibraryExA,
			(FARPROC)LoadLibraryExW,
			(FARPROC)GetModuleHandleA,
			(FARPROC)GetModuleHandleExA,
			(FARPROC)GetModuleHandleW,
			(FARPROC)GetModuleHandleExW,
};

class JsNativeModule : public JsWrapperClass<void> {
protected:
public:
	inline static const wchar_t* class_name = L"NativeModule";

	JsNativeModule(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(this->jsNoConstructor, this);
	}

	JsValueRef construct(void* mod, bool del) override {
		JsValueRef obj;
		JS::JsCreateExternalObject(mod, [](void*) {
			}, &obj);

		JS::JsSetPrototype(obj, getPrototype());
		Chakra::SetPropertyNumber(obj, L"handle", (double)(uintptr_t)mod);
		return obj;
	}

	void prepareFunctions() override {
		// static
		Chakra::DefineFunc(constructor, getCallback, XW("get"), this);
		Chakra::DefineFunc(prototype, defaultToString, XW("toString"), this);
		Chakra::DefineFunc(prototype, callCallback, XW("call"), this);
		Chakra::SetPropertyNumber(prototype, XW("handle"), 0.0);

	};

	static JsValueRef CALLBACK getCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return JS_INVALID_REFERENCE;

		if (!JsScript::getThis()->getPlugin()->isTrusted() && !JsScript::getThis()->getPlugin()->hasPermission(JsPlugin::UserPermission::SYSTEM_ACCESS)) {
			return Chakra::GetNull();
		}

		std::wstring name = Chakra::GetString(arguments[1]);
		auto thi = reinterpret_cast<JsWrapperClass*>(callbackState);

		static std::array<HMODULE, 2> banList = {
			GetModuleHandleA(NULL),
			Latite::get().dllInst,
		};

		auto handle = GetModuleHandleW(name.c_str());
		for (auto& ban : banList) {
			if (handle == ban) {
				handle = 0;
			}
		}

		if (!handle) return Chakra::GetNull();
		return thi->construct(handle, false);
	}

	static JsValueRef CALLBACK callCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		if (!Chakra::VerifyArgCount(argCount, 3, true, true)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsString} })) return JS_INVALID_REFERENCE;

		if (argCount > 18) {
			Chakra::ThrowError(XW("NativeModule.call can only accept 15 native arguments."));
			return JS_INVALID_REFERENCE;
		}

		using any_func_int64_t = int64_t(__stdcall*)(...);
		using any_func_int32_t = int32_t(__stdcall*)(...);
		using any_func_float64_t = double(__stdcall*)(...);
		using any_func_float32_t = float(__stdcall*)(...);
		auto thi = reinterpret_cast<JsWrapperClass*>(callbackState);

		std::wstring name = Chakra::GetString(arguments[1]);
		std::wstring type = Chakra::GetString(arguments[2]);

		HMODULE mod = (HMODULE)Get(arguments[0]);
		auto proc = GetProcAddress(mod, util::WStrToStr(name).c_str());

		if (!proc) {
			Chakra::ThrowError(XW("Could not find function ") + name);
			return JS_INVALID_REFERENCE;
		}

		if (!JsScript::getThis()->getPlugin()->isTrusted())
			for (auto& banned : banList) {
				if (proc == banned) {
					Chakra::ThrowError(XW("This function is banned in non-trusted plugins."));
					return JS_INVALID_REFERENCE;
				}
			}

		std::array<void*, 15> args = {};
		for (size_t i = 0; i < args.size(); i++) {
			auto arg = Chakra::TryGet(arguments, argCount, 3 + i);;
			if (arg != JS_INVALID_REFERENCE) {
				JsValueType type;
				JS::JsGetValueType(arg, &type);

				if (type == JsString) {
					const wchar_t* wstr;
					size_t strLength;
					auto str = JS::JsStringToPointer(arg, &wstr, &strLength);
					args[i] = (void*)wstr;
				}
				else if (type == JsNumber) {
					auto num = Chakra::GetNumber(arg);
					args[i] = (void*)(uint64_t)num;
				}
			}
		}


		if (type == XW("int64")) {
			// int64_t
			auto res = ((any_func_int64_t)proc)(args[0], args[1], args[2], args[3], args[4], args[5],
				args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
			return Chakra::MakeDouble(res);
		}
		else if (type == XW("int32")) {
			// int
			auto res = ((any_func_int32_t)proc)(args[0], args[1], args[2], args[3], args[4], args[5],
				args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
			return Chakra::MakeInt(res);
		}
		else if (type == XW("float32")) {
			// float
			auto res = ((any_func_float32_t)proc)(args[0], args[1], args[2], args[3], args[4], args[5],
				args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
			return Chakra::MakeDouble(res);
		}
		else if (type == XW("float64")) {
			// double
			auto res = ((any_func_float64_t)proc)(args[0], args[1], args[2], args[3], args[4], args[5],
				args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
			return Chakra::MakeDouble(res);
		}

		// void
		auto res = ((any_func_int64_t)proc)(args[0], args[1], args[2], args[3], args[4], args[5],
			args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
		return Chakra::GetUndefined();
	}
};
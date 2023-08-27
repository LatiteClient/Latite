#pragma once
#include "chakra/ChakraCore.h"
//#define USE_EDGEMODE_JSRT
//#include <jsrt.h>
#include "util.h"
#include "DxUtil.h"
#include "LMath.h"
#include "sdk/version.h"

// this is needed because we can't "link" ChakraCore with this DLL, as the path wouldn't be specified
#define FUNC(name) using _Type_ ## name = decltype(&name); inline static _Type_ ## name  name = (_Type_ ## name)Chakra::pass(#name)

class Chakra {
public:
	inline static HMODULE mod = 0;
	static FARPROC pass(const char* name);

	struct ParamContainer {
		JsValueRef val;
		JsValueType type;
		bool isOptional = false;

		ParamContainer(JsValueRef val, JsValueType type, bool optional = false) : val(val), type(type), isOptional(optional) {}
	};

	struct Result {
		bool success;
		std::wstring str;

		operator bool() {
			return success;
		}
	};

	static void StartDebugging(JsRuntimeHandle runtime, JsDiagDebugEventCallback callback, void* callbackState);
	static bool StopDebugging(JsRuntimeHandle runtime, void** callbackState);


	static void SetProperty(JsValueRef ref, std::wstring name, JsValueRef value, bool strict = false);
	static void SetPropertyString(JsValueRef ref, std::wstring name, std::wstring value, bool strict = false);
	static void SetPropertyNumber(JsValueRef ref, std::wstring name, double value, bool strict = false);
	static void SetPropertyBool(JsValueRef ref, std::wstring name, bool value, bool strict = false);
	static void SetPropertyObject(JsValueRef ref, std::wstring name, JsValueRef obj, bool strict = false);

	unsigned int GetRefCount(JsValueRef obj);
	
	static JsValueRef GetProperty(JsValueRef obj, std::wstring name);

	static JsValueRef TryGet(JsValueRef* args, unsigned short count, unsigned short idx);

	static void DefineFunc(JsValueRef obj, JsNativeFunction func, std::wstring name, void* state = nullptr);
	static std::wstring GetTypeName(JsValueType type);

	static JsValueRef MakeString(std::wstring const& ws);
	static JsValueRef MakeInt(int num);
	static JsValueRef MakeDouble(double num);

	static std::wstring GetStringProperty(JsValueRef ref, std::wstring name);

	static bool GetBoolProperty(JsValueRef ref, std::wstring name);
	static double GetNumberProperty(JsValueRef ref, std::wstring name);
	static double GetNumber(JsValueRef ref);
	static int GetIntProperty(JsValueRef ref, std::wstring name);
	static int GetInt(JsValueRef ref);
	static bool GetBool(JsValueRef ref);
	static std::wstring GetString(JsValueRef ref);

	static d2d::Rect GetRectFromJs(JsValueRef obj);
	static Vec2 GetVec2FromJs(JsValueRef obj);
	static Vec3 GetVec3FromJs(JsValueRef obj);
	static d2d::Color GetColorFromJs(JsValueRef obj);

	static JsValueRef GetUndefined();
	static JsValueRef GetTrue();
	static JsValueRef GetFalse();
	static JsValueRef GetNull();

	[[deprecated]] static void HandleErrors(JsErrorCode err);

	static std::wstring ToString(JsValueRef ref);
	static Result VerifyParameters(std::initializer_list<ParamContainer> params, bool autoThrow = true);
	static Result VerifyArgCount(unsigned short has, unsigned short expected, bool autoThrow = true);
	static void ThrowError(std::wstring message);
	static void Release(JsValueRef ref);
};

class JS {
public:
	//FUNC(JsProjectWinRTNamespace);
	//FUNC(JsSetProjectionEnqueueCallback);
	//FUNC(JsDiscardBackgroundParse);
	//FUNC(JsInitializeJITServer);
	//FUNC(JsQueueBackgroundParse);
	//FUNC(DllCanUnloadNow);
	//FUNC(DllGetClassObject);
	//FUNC(DllRegisterServer);
	//FUNC(DllUnregisterServer);
	FUNC(JsAddRef);
	FUNC(JsBoolToBoolean);
	FUNC(JsBooleanToBool);
	FUNC(JsCallFunction);
	FUNC(JsCollectGarbage);
	FUNC(JsConstructObject);
	FUNC(JsConvertValueToBoolean);
	FUNC(JsConvertValueToNumber);
	FUNC(JsConvertValueToObject);
	FUNC(JsConvertValueToString);
	FUNC(JsCreateArray);
	FUNC(JsCreateArrayBuffer);
	FUNC(JsCreateContext);
	FUNC(JsCreateDataView);
	FUNC(JsCreateError);
	FUNC(JsCreateExternalArrayBuffer);
	FUNC(JsCreateExternalObject);
	FUNC(JsCreateFunction);
	FUNC(JsCreateNamedFunction);
	FUNC(JsCreateObject);
	FUNC(JsCreateRangeError);
	FUNC(JsCreateReferenceError);
	FUNC(JsCreateRuntime);
	FUNC(JsCreateSymbol);
	FUNC(JsCreateSyntaxError);
	//FUNC(JsCreateThreadService);
	FUNC(JsCreateTypeError);
	FUNC(JsCreateTypedArray);
	FUNC(JsCreateURIError);
	FUNC(JsDefineProperty);
	FUNC(JsDeleteIndexedProperty);
	FUNC(JsDeleteProperty);
	FUNC(JsDisableRuntimeExecution);
	FUNC(JsDisposeRuntime);
	FUNC(JsDoubleToNumber);
	FUNC(JsEnableRuntimeExecution);
	//FUNC(JsEnumerateHeap);
	FUNC(JsEquals);
	FUNC(JsGetAndClearException);
	FUNC(JsGetArrayBufferStorage);
	FUNC(JsGetContextData);
	FUNC(JsGetContextOfObject);
	FUNC(JsGetCurrentContext);
	FUNC(JsGetDataViewStorage);
	FUNC(JsGetExtensionAllowed);
	FUNC(JsGetExternalData);
	FUNC(JsGetFalseValue);
	FUNC(JsGetGlobalObject);
	FUNC(JsGetIndexedPropertiesExternalData);
	FUNC(JsGetIndexedProperty);
	FUNC(JsGetNullValue);
	FUNC(JsGetOwnPropertyDescriptor);
	FUNC(JsGetOwnPropertyNames);
	FUNC(JsGetOwnPropertySymbols);
	FUNC(JsGetProperty);
	FUNC(JsGetPropertyIdFromName);
	FUNC(JsGetPropertyIdFromSymbol);
	FUNC(JsGetPropertyIdType);
	FUNC(JsGetPropertyNameFromId);
	FUNC(JsGetPrototype);
	FUNC(JsGetRuntime);
	FUNC(JsGetRuntimeMemoryLimit);
	FUNC(JsGetRuntimeMemoryUsage);
	FUNC(JsGetStringLength);
	FUNC(JsGetSymbolFromPropertyId);
	FUNC(JsGetTrueValue);
	FUNC(JsGetTypedArrayInfo);
	FUNC(JsGetTypedArrayStorage);
	FUNC(JsGetUndefinedValue);
	FUNC(JsGetValueType);
	FUNC(JsHasException);
	FUNC(JsHasExternalData);
	FUNC(JsHasIndexedPropertiesExternalData);
	FUNC(JsHasIndexedProperty);
	FUNC(JsHasProperty);
	FUNC(JsIdle);
	//FUNC(JsInspectableToObject);
	FUNC(JsInstanceOf);
	FUNC(JsIntToNumber);
	//FUNC(JsIsEnumeratingHeap);
	FUNC(JsIsRuntimeExecutionDisabled);
	FUNC(JsNumberToDouble);
	FUNC(JsNumberToInt);
	//FUNC(JsObjectToInspectable);
	FUNC(JsParseScript);
	FUNC(JsParseScriptWithAttributes);
	//FUNC(JsParseSerializedScript);
	FUNC(JsParseSerializedScriptWithCallback);
	FUNC(JsPointerToString);
	FUNC(JsPreventExtension);
	FUNC(JsRelease);
	FUNC(JsRunScript);
	FUNC(JsRunSerializedScript);
	FUNC(JsRunSerializedScriptWithCallback);
	FUNC(JsSerializeScript);
	FUNC(JsSetContextData);
	FUNC(JsSetCurrentContext);
	FUNC(JsSetException);
	FUNC(JsSetExternalData);
	FUNC(JsSetIndexedPropertiesToExternalData);
	FUNC(JsSetIndexedProperty);
	FUNC(JsSetObjectBeforeCollectCallback);
	FUNC(JsSetPromiseContinuationCallback);
	FUNC(JsSetProperty);
	FUNC(JsSetPrototype);
	FUNC(JsSetRuntimeBeforeCollectCallback);
	FUNC(JsSetRuntimeMemoryAllocationCallback);
	FUNC(JsSetRuntimeMemoryLimit);

	// ChakraCore ONLY
	FUNC(JsDiagStartDebugging);
	// ChakraCore ONLY
	FUNC(JsDiagStopDebugging);

	//FUNC(JsStartDebugging);
	//FUNC(JsStartProfiling);
	//FUNC(JsStopProfiling);
	FUNC(JsStrictEquals);
	FUNC(JsStringToPointer);
	//FUNC(JsValueToVariant);
	//FUNC(JsVarAddRef);
	//FUNC(JsVarRelease);
	//FUNC(JsVarToExtension);
	//FUNC(JsVarToScriptDirect);
	//FUNC(JsVariantToValue);
};

#undef FUNC
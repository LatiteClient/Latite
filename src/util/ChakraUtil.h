#pragma once
#include "chakra/ChakraCore.h"
#include "util.h"
#include "DxUtil.h"
#include "LMath.h"

// this is needed because we can't "link" ChakraCore with this DLL, as the path wouldn't be specified
#define FUNC(name) using _Type_ ## name = decltype(&name); inline static _Type_ ## name  name = (_Type_ ## name)Chakra::pass(#name)


class Chakra {
public:
	inline static HMODULE mod = 0;

	static FARPROC pass(const char* name) {
		if (!mod) mod = LoadLibraryW((util::GetLatitePath()/"Assets"/"ChakraCore.dll").wstring().c_str());
		if (!mod) {
			MessageBoxA(NULL, "Assets\\ChakraCore.dll could not be found!", "Error", MB_ICONERROR | MB_OK);
			exit(1);
		}
		return GetProcAddress(mod, name);
	}

	struct ParamContainer {
		JsValueRef val;
		JsValueType type;

		ParamContainer(JsValueRef val, JsValueType type) : val(val), type(type) {}
	};

	struct Result {
		bool success;
		std::wstring str;

		operator bool() {
			return success;
		}
	};

	// Off-naming convention, but i'm not renaming everything

	static void SetPropertyString(JsValueRef ref, std::wstring name, std::wstring value, bool strict = false);
	static void SetPropertyNumber(JsValueRef ref, std::wstring name, double value, bool strict = false);
	static void SetPropertyBool(JsValueRef ref, std::wstring name, bool value, bool strict = false);
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

	static std::wstring ToString(JsValueRef ref);
	static Result VerifyParameters(std::initializer_list<ParamContainer> params, bool autoThrow = true);
	static Result VerifyArgCount(unsigned short has, unsigned short expected, bool autoThrow = true);
	static void ThrowError(std::wstring message);
	static void Release(JsValueRef ref);
};

class JS {
public:
	FUNC(JsAddRef);
	FUNC(JsAllocRawData);
	FUNC(JsBoolToBoolean);
	FUNC(JsBooleanToBool);
	FUNC(JsCallFunction);
	FUNC(JsCloneObject);
	FUNC(JsCollectGarbage);
	FUNC(JsConnectJITProcess);
	FUNC(JsConstructObject);
	FUNC(JsConvertValueToBoolean);
	FUNC(JsConvertValueToNumber);
	FUNC(JsConvertValueToObject);
	FUNC(JsConvertValueToString);
	FUNC(JsCopyPropertyId);
	FUNC(JsCopyString);
	FUNC(JsCopyStringOneByte);
	FUNC(JsCopyStringUtf16);
	FUNC(JsCreateArray);
	FUNC(JsCreateArrayBuffer);
	FUNC(JsCreateContext);
	FUNC(JsCreateCustomExternalObject);
	FUNC(JsCreateDataView);
	FUNC(JsCreateEnhancedFunction);
	FUNC(JsCreateError);
	FUNC(JsCreateExternalArrayBuffer);
	FUNC(JsCreateExternalObject);
	FUNC(JsCreateExternalObjectWithPrototype);
	FUNC(JsCreateFunction);
	FUNC(JsCreateNamedFunction);
	FUNC(JsCreateObject);
	FUNC(JsCreatePromise);
	FUNC(JsCreatePropertyId);
	FUNC(JsCreatePropertyString);
	FUNC(JsCreateRangeError);
	FUNC(JsCreateReferenceError);
	FUNC(JsCreateRuntime);
	FUNC(JsCreateSharedArrayBufferWithSharedContent);
	FUNC(JsCreateString);
	FUNC(JsCreateStringUtf16);
	FUNC(JsCreateSymbol);
	FUNC(JsCreateSyntaxError);
	FUNC(JsCreateTracedExternalObject);
	FUNC(JsCreateTypeError);
	FUNC(JsCreateTypedArray);
	FUNC(JsCreateURIError);
	FUNC(JsCreateWeakReference);
	FUNC(JsDefineProperty);
	FUNC(JsDeleteIndexedProperty);
	FUNC(JsDeleteProperty);
	FUNC(JsDeserializeParserState);
	FUNC(JsDetachArrayBuffer);
	FUNC(JsDiagEvaluate);
	FUNC(JsDiagGetBreakOnException);
	FUNC(JsDiagGetBreakpoints);
	FUNC(JsDiagGetFunctionPosition);
	FUNC(JsDiagGetObjectFromHandle);
	FUNC(JsDiagGetProperties);
	FUNC(JsDiagGetScripts);
	FUNC(JsDiagGetSource);
	FUNC(JsDiagGetStackProperties);
	FUNC(JsDiagGetStackTrace);
	FUNC(JsDiagRemoveBreakpoint);
	FUNC(JsDiagRequestAsyncBreak);
	FUNC(JsDiagSetBreakOnException);
	FUNC(JsDiagSetBreakpoint);
	FUNC(JsDiagSetStepType);
	FUNC(JsDiagStartDebugging);
	FUNC(JsDiagStopDebugging);
	FUNC(JsDisableRuntimeExecution);
	FUNC(JsDiscardBackgroundParse_Experimental);
	FUNC(JsDisposeRuntime);
	FUNC(JsDoubleToNumber);
	FUNC(JsEnableOOPJIT);
	FUNC(JsEnableRuntimeExecution);
	FUNC(JsEquals);
	FUNC(JsExecuteBackgroundParse_Experimental);
	FUNC(JsExternalizeArrayBuffer);
	FUNC(JsGetAndClearException);
	FUNC(JsGetAndClearExceptionWithMetadata);
	FUNC(JsGetArrayBufferExtraInfo);
	FUNC(JsGetArrayBufferFreeFunction);
	FUNC(JsGetArrayBufferStorage);
	FUNC(JsGetArrayEntriesFunction);
	FUNC(JsGetArrayForEachFunction);
	FUNC(JsGetArrayKeysFunction);
	FUNC(JsGetArrayValuesFunction);
	FUNC(JsGetContextData);
	FUNC(JsGetContextOfObject);
	FUNC(JsGetCurrentContext);
	FUNC(JsGetDataViewInfo);
	FUNC(JsGetDataViewStorage);
	FUNC(JsGetEmbedderData);
	FUNC(JsGetErrorPrototype);
	FUNC(JsGetExtensionAllowed);
	FUNC(JsGetExternalData);
	FUNC(JsGetFalseValue);
	FUNC(JsGetGlobalObject);
	FUNC(JsGetIndexedPropertiesExternalData);
	FUNC(JsGetIndexedProperty);
	FUNC(JsGetIteratorPrototype);
	FUNC(JsGetModuleHostInfo);
	FUNC(JsGetModuleNamespace);
	FUNC(JsGetNullValue);
	FUNC(JsGetOwnPropertyDescriptor);
	FUNC(JsGetOwnPropertyNames);
	FUNC(JsGetOwnPropertySymbols);
	FUNC(JsGetPromiseResult);
	FUNC(JsGetPromiseState);
	FUNC(JsGetProperty);
	FUNC(JsGetPropertyIdFromName);
	FUNC(JsGetPropertyIdFromSymbol);
	FUNC(JsGetPropertyIdSymbolIterator);
	FUNC(JsGetPropertyIdType);
	FUNC(JsGetPropertyNameFromId);
	FUNC(JsGetPrototype);
	FUNC(JsGetProxyProperties);
	FUNC(JsGetRuntime);
	FUNC(JsGetRuntimeMemoryLimit);
	FUNC(JsGetRuntimeMemoryUsage);
	FUNC(JsGetSharedArrayBufferContent);
	FUNC(JsGetStringLength);
	FUNC(JsGetSymbolFromPropertyId);
	FUNC(JsGetTrueValue);
	FUNC(JsGetTypedArrayInfo);
	FUNC(JsGetTypedArrayStorage);
	FUNC(JsGetUndefinedValue);
	FUNC(JsGetValueType);
	FUNC(JsGetWeakReferenceValue);
	FUNC(JsHasException);
	FUNC(JsHasExternalData);
	FUNC(JsHasIndexedPropertiesExternalData);
	FUNC(JsHasIndexedProperty);
	FUNC(JsHasOwnItem);
	FUNC(JsHasOwnProperty);
	FUNC(JsHasProperty);
	FUNC(JsIdle);
	FUNC(JsInitializeModuleRecord);
	FUNC(JsInstanceOf);
	FUNC(JsIntToNumber);
	FUNC(JsIsCallable);
	FUNC(JsIsConstructor);
	FUNC(JsIsRuntimeExecutionDisabled);
	FUNC(JsLessThan);
	FUNC(JsLessThanOrEqual);
	FUNC(JsModuleEvaluation);
	FUNC(JsNumberToDouble);
	FUNC(JsNumberToInt);
	FUNC(JsObjectDefineProperty);
	FUNC(JsObjectDefinePropertyFull);
	FUNC(JsObjectDeleteProperty);
	FUNC(JsObjectGetOwnPropertyDescriptor);
	FUNC(JsObjectGetProperty);
	FUNC(JsObjectHasOwnProperty);
	FUNC(JsObjectHasProperty);
	FUNC(JsObjectSetProperty);
	FUNC(JsParse);
	FUNC(JsParseModuleSource);
	FUNC(JsParseScript);
	FUNC(JsParseScriptWithAttributes);
	FUNC(JsParseSerialized);
	FUNC(JsParseSerializedScript);
	FUNC(JsParseSerializedScriptWithCallback);
	FUNC(JsPointerToString);
	FUNC(JsPreventExtension);
	FUNC(JsPrivateDeleteProperty);
	FUNC(JsPrivateGetProperty);
	FUNC(JsPrivateHasProperty);
	FUNC(JsPrivateSetProperty);
	FUNC(JsQueueBackgroundParse_Experimental);
	FUNC(JsRelease);
	FUNC(JsReleaseSharedArrayBufferContentHandle);
	FUNC(JsRun);
	FUNC(JsRunScript);
	FUNC(JsRunScriptWithParserState);
	FUNC(JsRunSerialized);
	FUNC(JsRunSerializedScript);
	FUNC(JsRunSerializedScriptWithCallback);
	FUNC(JsSerialize);
	FUNC(JsSerializeParserState);
	FUNC(JsSerializeScript);
	FUNC(JsSetArrayBufferExtraInfo);
	FUNC(JsSetContextData);
	FUNC(JsSetCurrentContext);
	FUNC(JsSetEmbedderData);
	FUNC(JsSetException);
	FUNC(JsSetExternalData);
	FUNC(JsSetHostPromiseRejectionTracker);
	FUNC(JsSetIndexedPropertiesToExternalData);
	FUNC(JsSetIndexedProperty);
	FUNC(JsSetModuleHostInfo);
	FUNC(JsSetObjectBeforeCollectCallback);
	FUNC(JsSetPromiseContinuationCallback);
	FUNC(JsSetProperty);
	FUNC(JsSetPrototype);
	FUNC(JsSetRuntimeBeforeCollectCallback);
	FUNC(JsSetRuntimeBeforeSweepCallback);
	FUNC(JsSetRuntimeDomWrapperTracingCallbacks);
	FUNC(JsSetRuntimeMemoryAllocationCallback);
	FUNC(JsSetRuntimeMemoryLimit);
	FUNC(JsStrictEquals);
	FUNC(JsStringToPointer);
	FUNC(JsTTDCheckAndAssertIfTTDRunning);
	FUNC(JsTTDCreateContext);
	FUNC(JsTTDCreateRecordRuntime);
	FUNC(JsTTDCreateReplayRuntime);
	FUNC(JsTTDDiagSetAutoTraceStatus);
	FUNC(JsTTDDiagWriteLog);
	FUNC(JsTTDGetPreviousSnapshotInterval);
	FUNC(JsTTDGetSnapShotBoundInterval);
	FUNC(JsTTDGetSnapTimeTopLevelEventMove);
	FUNC(JsTTDHostExit);
	FUNC(JsTTDMoveToTopLevelEvent);
	FUNC(JsTTDNotifyContextDestroy);
	FUNC(JsTTDNotifyLongLivedReferenceAdd);
	FUNC(JsTTDNotifyYield);
	FUNC(JsTTDPauseTimeTravelBeforeRuntimeOperation);
	FUNC(JsTTDPreExecuteSnapShotInterval);
	FUNC(JsTTDRawBufferAsyncModificationRegister);
	FUNC(JsTTDRawBufferAsyncModifyComplete);
	FUNC(JsTTDRawBufferCopySyncIndirect);
	FUNC(JsTTDRawBufferModifySyncIndirect);
	FUNC(JsTTDReStartTimeTravelAfterRuntimeOperation);
	FUNC(JsTTDReplayExecution);
	FUNC(JsTTDStart);
	FUNC(JsTTDStop);
	FUNC(JsTraceExternalReference);
	FUNC(JsVarDeserializer);
	FUNC(JsVarDeserializerFree);
	FUNC(JsVarDeserializerReadBytes);
	FUNC(JsVarDeserializerReadRawBytes);
	FUNC(JsVarDeserializerReadValue);
	FUNC(JsVarDeserializerSetTransferableVars);
	FUNC(JsVarSerializer);
	FUNC(JsVarSerializerDetachArrayBuffer);
	FUNC(JsVarSerializerFree);
	FUNC(JsVarSerializerReleaseData);
	FUNC(JsVarSerializerSetTransferableVars);
	FUNC(JsVarSerializerWriteRawBytes);
	FUNC(JsVarSerializerWriteValue);
};

#undef FUNC
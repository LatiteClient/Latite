#include "pch.h"
#include "JsSettingClass.h"
#include "JsColor.h"
#include "JsVec2.h"
#include "../../JsPlugin.h"

JsValueRef JsSettingClass::getValueCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto thi = reinterpret_cast<JsSettingClass*>(callbackState);
	auto set = Get(arguments[0]);

	// UGH this SUCKS!!!!!!!
	switch ((Setting::Type)set->value->index()) {
	case Setting::Type::Bool:
		return std::get<BoolValue>(*set->value) ? Chakra::GetTrue() : Chakra::GetFalse();
	case Setting::Type::Int:
		return Chakra::MakeInt(std::get<IntValue>(*set->value).value);
	case Setting::Type::Float:
		return Chakra::MakeDouble(static_cast<double>(std::get<FloatValue>(*set->value).value));
	case Setting::Type::Key:
		return Chakra::MakeInt(std::get<KeyValue>(*set->value).value);
	case Setting::Type::Enum:
		return Chakra::MakeInt(std::get<EnumValue>(*set->value).val);
	case Setting::Type::Color:
	{
		auto colClass = thi->owner->getClass<JsColor>();
		if (colClass) {
			return colClass->construct(d2d::Color(std::get<ColorValue>(*set->value).getMainColor()));
		}
		break;
	}
	case Setting::Type::Vec2:
	{
		auto colClass = thi->owner->getClass<JsVec2>();
		if (colClass) {
			return colClass->construct(Vec2(std::get<Vec2Value>(*set->value).x, std::get<Vec2Value>(*set->value).y));
		}
		break;
	}
	case Setting::Type::Text:
	{
		return Chakra::MakeString(std::get<TextValue>(*set->value).str);
	}

	}
	return Chakra::GetNull();
}

JsValueRef JsSettingClass::setValueCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto thi = reinterpret_cast<JsSettingClass*>(callbackState);
	auto set = Get(arguments[0]);

	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;

	auto setVal = arguments[1];

	JsValueType argType;
	JS::JsGetValueType(setVal, &argType);

	if (argType == JsNull || argType == JsUndefined) {
		return Chakra::GetUndefined();
	}

	auto undef = Chakra::GetUndefined();

	switch ((Setting::Type)set->value->index()) {
	case Setting::Type::Bool:
		if (argType == JsBoolean) {
			std::get<BoolValue>(*set->value).value = Chakra::GetBool(setVal);
		}
		return undef;
	case Setting::Type::Int:
		if (argType == JsNumber) {
			std::get<IntValue>(*set->value).value = Chakra::GetInt(setVal);
		}
		return undef;
	case Setting::Type::Float:
		if (argType == JsNumber) {
			std::get<FloatValue>(*set->value).value = Chakra::GetNumber(setVal);
		}
		return undef;
	case Setting::Type::Text:
		if (argType == JsString) {
			std::get<TextValue>(*set->value).str = Chakra::GetString(setVal);
		}
		return undef;

	case Setting::Type::Color:
		if (argType == JsObject) {
			auto col = JsColor::ToColor(setVal);
			std::get<ColorValue>(*set->value).getMainColor() = StoredColor(col.r, col.g, col.b, col.a);
		}
		return undef;
	case Setting::Type::Enum:
		if (argType == JsNumber) {
			std::get<EnumValue>(*set->value).val = Chakra::GetInt(setVal);
		}
		return undef;
	case Setting::Type::Key:
		if (argType == JsNumber) {
			std::get<KeyValue>(*set->value).value = Chakra::GetInt(setVal);
		}
		return undef;
	case Setting::Type::Vec2:
		if (argType == JsObject) {
			auto vec = JsVec2::ToVec2(setVal);
			std::get<Vec2Value>(*set->value).x = vec.x;
			std::get<Vec2Value>(*set->value).y = vec.y;
		}
		return undef;
	default:
		Chakra::ThrowError(XW("Unsupported setting type for setValue"));
		return JS_INVALID_REFERENCE;
	}
}

JsValueRef JsSettingClass::setCondition(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState){
	if (!Chakra::VerifyArgCount(argCount, 2, true, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {Chakra::TryGet(arguments, argCount, 2), JsBoolean, true}})) return JS_INVALID_REFERENCE;
	
	auto thi = reinterpret_cast<JsSettingClass*>(callbackState);
	auto set = Get(arguments[0]);
	bool setBool = true;
	if (Chakra::TryGet(arguments, argCount, 2)) {
		setBool = Chakra::GetBool(arguments[2]);
	}

	set->condition = Setting::Condition(util::WStrToStr(Chakra::GetString(arguments[1])), Setting::Condition::EQUALS, { static_cast<int>(setBool)});
	return Chakra::GetUndefined();
}

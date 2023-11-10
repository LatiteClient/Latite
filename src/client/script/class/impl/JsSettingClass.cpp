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
			return colClass->construct(d2d::Color(std::get<ColorValue>(*set->value).color1));
		}
	}
	case Setting::Type::Vec2:
	{
		auto colClass = thi->owner->getClass<JsVec2>();
		if (colClass) {
			return colClass->construct(Vec2(std::get<Vec2Value>(*set->value).x, std::get<Vec2Value>(*set->value).y));
		}
	}
	case Setting::Type::Text:
	{
		return Chakra::MakeString(std::get<TextValue>(*set->value).str);
	}

	}
	return Chakra::GetNull();
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

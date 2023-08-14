#include "JsSettingClass.h"
#include "JsColor.h"
#include "JsVec2.h"
#include "util/DxUtil.h"
#include "util/LMath.h"
#include "../../JsScript.h"

JsValueRef JsSettingClass::getValueCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	auto thi = reinterpret_cast<JsSettingClass*>(callbackState);
	auto set = ToSetting(arguments[0]);

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
		auto colClass = thi->owner->findClass<JsColor>(L"Color");
		if (colClass) {
			return colClass->construct(d2d::Color(std::get<ColorValue>(*set->value).color1));
		}
	}
	case Setting::Type::Vec2:
	{
		auto colClass = thi->owner->findClass<JsVec2>(L"Vector2");
		if (colClass) {
			return colClass->construct(Vec2(std::get<Vec2Value>(*set->value).x, std::get<Vec2Value>(*set->value).y));
		}
	}

	}
	return Chakra::GetNull();
}

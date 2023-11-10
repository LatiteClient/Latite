#pragma once
#include <string>
#include "util/ChakraUtil.h"

class JsLibrary {
public:
	JsLibrary(class JsPlugin* owner, std::wstring const& inclName) : includeName(inclName), owner(owner) {}

	virtual JsValueRef initialize(JsValueRef parentObj) = 0;
	virtual bool shouldInclude(std::wstring const& str) {
		return this->includeName == str;
	}
protected:
	class JsPlugin* owner;
	std::wstring includeName;
};

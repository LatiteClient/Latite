#pragma once
#include "util/ChakraUtil.h"

class ScriptingObject {
public:
	int id;

	// Object is already created.
	ScriptingObject(int id) { this->id = id; JS::JsCreateObject(&object); }

	virtual ~ScriptingObject();
	virtual void initialize(JsContextRef ctx, JsValueRef parentObj) = 0;
protected:
	JsValueRef object = JS_INVALID_REFERENCE;
};
#pragma once
#include "util/ChakraUtil.h"
#include "client/event/Listener.h"

class ScriptingObject : public Listener {
public:
	int id;
	const wchar_t* objName;

	// Object is already created.
	// you NEED to call initialize shortly after constructing it or there is a chance the garbage collector will do a funny
	ScriptingObject(int id, const wchar_t* name) : objName(name) { this->id = id; JS::JsCreateObject(&object); }

	virtual ~ScriptingObject();

	virtual void initialize(JsContextRef ctx, JsValueRef parentObj) = 0;

	JsValueRef object = JS_INVALID_REFERENCE;
protected:
};
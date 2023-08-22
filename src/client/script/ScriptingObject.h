/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "util/ChakraUtil.h"
#include "api/eventing/Listenable.h"

class ScriptingObject : public Listener {
public:
	int id;
	const wchar_t* objName;

	// Object is already created.
	ScriptingObject(int id, const wchar_t* name) : objName(name) { this->id = id; JS::JsCreateObject(&object); }

	~ScriptingObject();

	virtual void initialize(JsContextRef ctx, JsValueRef parentObj) = 0;

	JsValueRef object = JS_INVALID_REFERENCE;
protected:
};
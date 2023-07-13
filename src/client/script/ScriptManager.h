#pragma once
#include "api/manager/Manager.h"
#include "JsScript.h"

class ScriptManager final : public Manager<JsScript> {
	std::vector<std::shared_ptr<JsScript>> scripts;
private:
};
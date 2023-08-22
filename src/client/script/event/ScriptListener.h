#pragma once
#include "api/eventing/Listenable.h"

class ScriptListener final : public Listener {
public:
	ScriptListener();
	ScriptListener(ScriptListener&) = delete;
	ScriptListener(ScriptListener&&) = delete;

	~ScriptListener() = default;


};
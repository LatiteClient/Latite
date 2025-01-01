#pragma once
#include <client/feature/module/Module.h>

class AutoGG : public Module {
public:
	AutoGG();
	virtual ~AutoGG() {};

	void onText(Event&);

private:
	ValueType useCustomMessage = BoolValue(false);
	ValueType customMessage = TextValue(L"ggs!");
};
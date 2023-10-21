#pragma once
#include "client/feature/module/Module.h"
#include <client/event/impl/PerspectiveEvent.h>

class BehindYou : public Module {
public:
	BehindYou();
	virtual ~BehindYou() = default;

private:
	EnumData pers;
	ValueType bind = KeyValue('G');

	void onPerspective(Event& ev);
};
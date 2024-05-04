#pragma once
#include <client/feature/module/Module.h>

class Freelook : public Module {
public:
	Freelook();
	virtual ~Freelook() {};

	void onCameraUpdate(Event&);
	void onPerspective(Event&);

private:
};
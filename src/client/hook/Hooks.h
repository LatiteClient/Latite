#pragma once
#include "Hook.h"
#include "api/manager/Manager.h"

class LatiteHooks final : public Manager<HookGroup> {
public:
	LatiteHooks() = default;
	~LatiteHooks() = default;

	void init();
	void uninit();

	void enable();
	void disable();
private:
};
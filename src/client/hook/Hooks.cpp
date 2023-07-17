#include "Hooks.h"

#include "impl/GeneralHooks.h"
#include "MinHook.h"

void LatiteHooks::init()
{
	MH_Initialize();

	this->mutex.lock();
	this->items.push_back(std::make_shared<GenericHooks>());
	this->mutex.unlock();
}

void LatiteHooks::uninit()
{
	MH_Uninitialize();
}

void LatiteHooks::enable()
{
	MH_EnableHook(MH_ALL_HOOKS);
}

void LatiteHooks::disable()
{
	MH_DisableHook(MH_ALL_HOOKS);
}

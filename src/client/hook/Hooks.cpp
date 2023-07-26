#include "Hooks.h"

#include "impl/GeneralHooks.h"
#include "impl/LevelRendererHooks.h"
#include "impl/OptionHooks.h"
#include "impl/DXHooks.h"
#include "impl/AppPlatformHooks.h"
#include "impl/MinecraftGameHooks.h"
#include "MinHook.h"

void LatiteHooks::init() {
	MH_Initialize();

	this->mutex.lock();
	this->items.push_back(std::make_shared<GenericHooks>());
	this->items.push_back(std::make_shared<LevelRendererHooks>());
	this->items.push_back(std::make_shared<OptionHooks>());
	this->items.push_back(std::make_shared<DXHooks>());
	this->items.push_back(std::make_shared<AppPlatformHooks>());
	this->items.push_back(std::make_shared<MinecraftGameHooks>());
	this->mutex.unlock();
}

void LatiteHooks::uninit() {
	MH_Uninitialize();
}

void LatiteHooks::enable() {
	MH_EnableHook(MH_ALL_HOOKS);
}

void LatiteHooks::disable() {
	MH_DisableHook(MH_ALL_HOOKS);
}

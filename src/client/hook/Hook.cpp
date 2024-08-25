#include "pch.h"
#include "Hook.h"
#include "util/logger.h"
#include "MinHook.h"
#include "vhook/vtable_hook.h"

Hook::Hook(uintptr_t target, void* detour, std::string const& hookName, bool tableSwap)
#ifdef LATITE_DEBUG
	: funcName(hookName)
#endif
{
	this->target = target;
	this->detour = detour;

	if (tableSwap) {
		auto res = vh::hook(reinterpret_cast<LPVOID*>(target), detour, &this->funcPtr);
		if (res != 0) {
#ifdef LATITE_DEBUG
			Logger::Warn("Creation of hook {} failed with status {}", this->funcName, vh::status_to_string(res));
#endif
		}
		return;
	}

	MH_STATUS res = MH_CreateHook(reinterpret_cast<LPVOID>(target), detour, &this->funcPtr);
	if (res != MH_OK) {
#ifdef LATITE_DEBUG
		Logger::Warn("Creation of hook {} failed with status {}", this->funcName, MH_StatusToString(res));
#endif
	}
}

HookGroup::HookGroup(std::string const& groupName) 
#ifdef LATITE_DEBUG
	: groupName(groupName)
#else
	// hopefully the name gets optimized away
	: groupName("")
#endif
{
}

std::shared_ptr<Hook> HookGroup::addHook(uintptr_t ptr, func_ptr_t detour, const char* name) {
#ifdef LATITE_DEBUG
	auto newHook = std::make_shared<Hook>(ptr, detour, name);
#else
	auto newHook = std::make_shared<Hook>(ptr, detour, "");
#endif
	hooks.emplace_back(newHook);
	return newHook;
}

std::shared_ptr<Hook> HookGroup::addTableSwapHook(uintptr_t ptr, func_ptr_t detour, const char* name) {
#ifdef LATITE_DEBUG
	auto newHook = std::make_shared<Hook>(ptr, detour, name, true);
#else
	auto newHook = std::make_shared<Hook>(ptr, detour, "", true);

#endif
	hooks.emplace_back(newHook);
	return newHook;
}

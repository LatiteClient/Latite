#include "Hook.h"
#include "util/logger.h"
#include "pch.h"
#include "MinHook.h"

Hook::Hook(uintptr_t target, void* detour, std::string const& hookName, bool tableSwap)
#ifdef LATITE_DEBUG
	: funcName(hookName)
#endif
{

	if (tableSwap) {
		// TODO: implement table swap hook
		return;
	}

	MH_STATUS res = MH_CreateHook(reinterpret_cast<LPVOID>(target), detour, &this->funcPtr);
	if (res != MH_OK) {
		Logger::Warn("Creation of hook {} failed with status {}", this->funcName, MH_StatusToString(res));
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
	auto newHook = std::make_shared<Hook>(ptr, detour, name);
	hooks.emplace_back(newHook);
	return newHook;
}

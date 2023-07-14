#pragma once
#include <string>
#include <cstdint>
#include "MinHook.h"
#include <vector>
#include <memory>

using func_ptr_t = void*;

class Hook {
private:
	void* funcPtr;
	void* detour;
	std::string funcName;
	bool tableSwapHook;
	uintptr_t target;

public:
	Hook(uintptr_t target, void* detour, const char* hookName = "A Hook", bool tableSwap = false);
	~Hook() = default;

	void enable() {
		MH_EnableHook(reinterpret_cast<void*>(target));
	}

	template <typename Func>
	Func getOFunc();
};

template<typename Func>
inline Func Hook::getOFunc()
{
	if (this->tableSwapHook) {
		return static_cast<Func>(this->funcPtr);
	}
	return static_cast<Func>(this->funcPtr);
}

class HookGroup
{
	const char* groupName;
public:
	std::vector<std::shared_ptr<Hook>> hooks;
	bool shouldVtableHook = false;

	HookGroup(const char* groupName = "Unnamed Hook Group");

	~HookGroup();

	std::shared_ptr<Hook> addHook(uintptr_t ptr, func_ptr_t detour, const char* name = "Unnamed Hook");
};

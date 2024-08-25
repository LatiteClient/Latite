#pragma once
#include <string>
#include <cstdint>
#include "MinHook.h"
#include <vector>
#include <memory>
#include <exception>

#include "util/Logger.h"

using func_ptr_t = void*;

class Hook {
private:
	void* funcPtr;
	void* detour;
	std::string funcName;
	bool tableSwapHook;
	uintptr_t target;

public:
	Hook(uintptr_t target, void* detour, std::string const& funcName = "A Hook", bool tableSwap = false);
	~Hook() = default;

	void enable() {
		if (target != 0) {
			const auto ret = MH_EnableHook(reinterpret_cast<void*>(target));

#ifdef LATITE_DEBUG
			if (ret != MH_OK)
				Logger::Warn("Enabling of hook {} failed with status {}", this->funcName, MH_StatusToString(ret));
#endif
		}
	}

	void disable() {
		if (target != 0) {
			const auto ret = MH_DisableHook(reinterpret_cast<void*>(target));
			
#ifdef LATITE_DEBUG
			if (ret != MH_OK)
				Logger::Warn("Disabling of hook {} failed with status {}", this->funcName, MH_StatusToString(ret));
#endif
		}
	}

	template <typename Func>
	Func oFunc();
};

template<typename Func>
inline Func Hook::oFunc()  {
	if (this->tableSwapHook) {
		return static_cast<Func>(this->funcPtr);
	}
	return static_cast<Func>(this->funcPtr);
}

class HookGroup
{
	std::string groupName;
public:
	std::vector<std::shared_ptr<Hook>> hooks;
	bool shouldVtableHook = false;

	HookGroup(std::string const& groupName = "Unnamed Hook Group");

	~HookGroup() = default;

	[[nodiscard]] std::shared_ptr<Hook> addHook(uintptr_t ptr, func_ptr_t detour, const char* name = "Unnamed Hook");
	[[nodiscard]] std::shared_ptr<Hook> addTableSwapHook(uintptr_t ptr, func_ptr_t detour, const char* name = "Unnamed Hook");
};

#include "pch.h"
#include "ItemStack.h"

SDK::ItemStack* SDK::ItemStack::constructFromBlock(void* storage, SDK::Block const& block, int count,
	SDK::CompoundTag const* userData) {
	using oFunc_t = ItemStack*(__fastcall*)(void*, Block const*, int, CompoundTag const*);
	auto fn = reinterpret_cast<oFunc_t>(Signatures::ItemStack_ItemStackBlock.result);
	return fn ? fn(storage, &block, count, userData) : nullptr;
}

void SDK::ItemStack::destruct() {
	using oFunc_t = void(__fastcall*)(ItemStackBase*);
	auto fn = reinterpret_cast<oFunc_t>(Signatures::ItemStackBase_destructor.result);
	if (fn) {
		fn(this);
	}
}

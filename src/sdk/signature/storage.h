#pragma once
#include "api/memory/signature/signature.h"
class SigImpl : public memory::signature_store {
public:

	explicit SigImpl(decltype(on_resolve) onResolve, std::string_view sig, std::string_view name) : signature_store("Minecraft.Windows.exe", onResolve, sig, name) {
	}
};

#include "storage_latest.h"
#include "storage_1_18_12.h"
#pragma once

#ifdef LATITE_DEBUG
#define API_NAMES
#endif

#include "api/memory/signature/signature.h"
using namespace mnem::sig_literals;

/*
#define MVSIG(...) ([]() -> SigImpl& {\
if (sdk::internalVers == sdk::VLATEST) return Signatures::__VA_ARGS__;\
else { return Signatures_1_18_12::__VA_ARGS__; }\
})()*/

class SigImpl : public memory::signature_store {
public:
	explicit SigImpl() : signature_store(nullptr, this->on_resolve, std::nullopt, "") {};

	explicit SigImpl(decltype(on_resolve) onResolve, mnem::signature sig, std::string_view name) : signature_store("Minecraft.Windows.exe", onResolve, sig, name) {
	}
};

#include "storage_latest.h"
//#include "storage_1_20_40.h"
//#include "storage_1_20_30.h"
//#include "storage_1_19_51.h"
//#include "storage_1_18_12.h"
//#include "storage_1_16_40.h"
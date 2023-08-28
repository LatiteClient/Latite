#include "OptionHooks.h"
#include "client/event/Eventing.h"
#include "client/event/impl/GammaEvent.h"
#include "pch.h"

namespace {
	std::shared_ptr<Hook> getGammaHook;
}

float OptionHooks::Options_getGamma(void* options) {
	auto o = getGammaHook->oFunc<decltype(&Options_getGamma)>()(options);
	GammaEvent ev{ o };
	Eventing::get().dispatch(ev);
	return ev.getGamma();
}

OptionHooks::OptionHooks() {
	getGammaHook = addHook(Signatures::Options_getGamma.result, Options_getGamma, "Options::getGamma");
}

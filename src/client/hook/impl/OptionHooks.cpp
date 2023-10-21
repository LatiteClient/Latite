#include "pch.h"
#include "OptionHooks.h"
#include "client/event/Eventing.h"
#include "client/event/impl/GammaEvent.h"
#include "client/event/impl/PerspectiveEvent.h"

namespace {
	std::shared_ptr<Hook> getGammaHook;
	std::shared_ptr<Hook> getPerspectiveHook;
}

float OptionHooks::Options_getGamma(void* options) {
	auto o = getGammaHook->oFunc<decltype(&Options_getGamma)>()(options);
	GammaEvent ev{ o };
	Eventing::get().dispatch(ev);
	return o;
}

int OptionHooks::Options_getPerspective(void* options) {
	auto o = getPerspectiveHook->oFunc<decltype(&Options_getPerspective)>()(options);
	PerspectiveEvent ev{ o };
	Eventing::get().dispatch(ev);
	return o;
}

OptionHooks::OptionHooks() {
	getGammaHook = addHook(Signatures::Options_getGamma.result, Options_getGamma, "Options::getGamma");
	getPerspectiveHook = addHook(Signatures::Options_getPerspective.result, Options_getPerspective, "Options::getPerspective");
}

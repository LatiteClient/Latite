#include "pch.h"
#include "OptionHooks.h"
#include "client/event/Eventing.h"
#include "client/event/impl/GammaEvent.h"
#include "client/event/impl/PerspectiveEvent.h"
#include "client/event/impl/HideHandEvent.h"
#include "client/event/impl/SensitivityEvent.h"

namespace {
	std::shared_ptr<Hook> getGammaHook;
	std::shared_ptr<Hook> getPerspectiveHook;
	std::shared_ptr<Hook> getHideHandHook;
	std::shared_ptr<Hook> getSensitivityHook;
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

bool OptionHooks::Options_getHideHand(void* options) {
	auto o = getHideHandHook->oFunc<decltype(&Options_getHideHand)>()(options);
	HideHandEvent ev{ o };
	Eventing::get().dispatch(ev);
	return o;
}

float OptionHooks::Options_getSensitivity(void* options) {
	auto o = getSensitivityHook->oFunc<decltype(&Options_getSensitivity)>()(options);
	SensitivityEvent ev{ o };
	Eventing::get().dispatch(ev);
	return o;
}

OptionHooks::OptionHooks() {
	getGammaHook = addHook(Signatures::Options_getGamma.result, Options_getGamma, "Options::getGamma");
	getPerspectiveHook = addHook(Signatures::Options_getPerspective.result, Options_getPerspective, "Options::getPerspective");
	getHideHandHook = addHook(Signatures::Options_getHideHand.result, Options_getHideHand, "Options::getHideHand");
	getSensitivityHook = addHook(Signatures::Options_getSensitivity.result, Options_getSensitivity, "Options::getSensitivity");
}

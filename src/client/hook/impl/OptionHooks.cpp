/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "OptionHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/Eventing.h"
#include "client/event/impl/GammaEvent.h"

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

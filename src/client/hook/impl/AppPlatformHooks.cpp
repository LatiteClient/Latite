/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "AppPlatformHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/Eventing.h"
#include "client/event/impl/FocusLostEvent.h"

namespace {
	std::shared_ptr<Hook> FocusLostHook;
}

int AppPlatformHooks::_fireAppFocusLost(SDK::AppPlatform* plat) {
	FocusLostEvent ev{};
	if (Eventing::get().dispatch(ev)) return 0;
	return FocusLostHook->oFunc<decltype(&_fireAppFocusLost)>()(plat);
}

AppPlatformHooks::AppPlatformHooks() {
	FocusLostHook = addHook(Signatures::AppPlatform__fireAppFocusLost.result, _fireAppFocusLost, "AppPlatform::_fireAppFocusLost");
}

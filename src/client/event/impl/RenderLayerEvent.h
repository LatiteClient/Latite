/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "sdk/common/client/gui/ScreenView.h"

class RenderLayerEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderLayerEvent);

	RenderLayerEvent(SDK::ScreenView* view) : view(view) {}

	[[nodiscard]] SDK::ScreenView* getScreenView() { return view; }
private:
	SDK::ScreenView* view;
};
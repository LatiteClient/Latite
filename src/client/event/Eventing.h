/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/EventManager.h"

class Eventing final : public IEventManager {
public:
	Eventing() = default;
	~Eventing() = default;
	//void init() override;

	// Substitute for Latite::getEventing
	[[nodiscard]] static Eventing& get();
};
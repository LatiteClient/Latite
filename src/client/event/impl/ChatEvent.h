/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class ChatEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(ChatEvent);

	[[nodiscard]] std::string& getText() { return *str; }

	ChatEvent(std::string& str) : str(&str) {
	}

protected:
	std::string* str;
};

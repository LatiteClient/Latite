#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class ChatEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(ChatEvent);

	[[nodiscard]] std::string& getText() { return *str; }

	ChatEvent(std::string& str) : str(&str) {
	}

protected:
	std::string* str;
};

#pragma once
#include "api/eventing/Event.h"
#include "util/chash.h"

class ChatEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(ChatEvent);

	[[nodiscard]] std::string& getText() { return *str; }

	ChatEvent(std::string& str) : str(&str) {
	}

protected:
	std::string* str;
};

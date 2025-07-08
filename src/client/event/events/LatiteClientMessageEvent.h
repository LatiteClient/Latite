#pragma once
class LatiteClientMessageEvent : public Event {
public:
	static const uint32_t hash = TOHASH(LatiteClientMessageEvent);

	LatiteClientMessageEvent(std::string const& msg) : message(msg) {}

	[[nodiscard]] std::string getMessage() { return message; }
private:
	std::string message;
};
#pragma once
class LatiteClientMessageEvent : public Event {
public:
	static const uint32_t hash = TOHASH(LatiteClientMessageEvent);

	LatiteClientMessageEvent(std::wstring const& msg) : message(msg) {}

	[[nodiscard]] std::wstring getMessage() { return message; }
private:
	std::wstring message;
};
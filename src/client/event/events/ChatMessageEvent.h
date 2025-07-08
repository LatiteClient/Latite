#pragma once
class ChatMessageEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(ChatMessageEvent);

	ChatMessageEvent(std::string const& msg) : message(msg) {}

	[[nodiscard]] std::string getMessage() {
		return message;
	}
private:
	std::string message;
};
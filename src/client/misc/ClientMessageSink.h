#pragma once
#include <vector>
#include <string>
#include <mutex>

// A class that handles client messages being sent to the player, so they can be printed safely.
class ClientMessageSink final {
public:
	ClientMessageSink() = default;
	~ClientMessageSink() = default;

	void doPrint(int numMessages);
	void push(std::wstring const& message);

	// ONLY call this if it's in the game thread.
	void display(std::wstring const& message);

private:
	std::mutex sinkLock;
	std::vector<std::wstring> messages;
};
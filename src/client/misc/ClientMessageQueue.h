#pragma once
#include <vector>
#include <string>
#include <mutex>

// A class that handles client messages being sent to the player, so they can be printed safely.
class ClientMessageQueue final {
public:
	ClientMessageQueue() = default;
	~ClientMessageQueue() = default;

	void doPrint(int numMessages);
	void push(std::string const& message);
	void push(std::wstring const& message);

	// ONLY call this if it's in the game thread.
	void display(std::string const& message);
	void display(std::wstring const& message);

private:
	std::mutex lock;
	std::vector<std::string> messages;
};
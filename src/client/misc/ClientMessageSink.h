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

private:
	std::mutex sinkLock;
	std::vector<std::string> messages;
};
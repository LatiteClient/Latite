#include "ClientMessageSink.h"
#include "util/Logger.h"
#include "pch.h"

void ClientMessageSink::doPrint(int numMessages)
{
	this->sinkLock.lock();
	for (size_t i = 0; i < this->messages.size() && i < numMessages; i++) {
		auto msg = messages.back();
		// TODO: actually print it lol
		Logger::info("{}", msg.c_str());
	}
	this->sinkLock.unlock();
}

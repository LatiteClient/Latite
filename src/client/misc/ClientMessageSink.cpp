#include "ClientMessageSink.h"
#include "util/Logger.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "pch.h"

void ClientMessageSink::doPrint(int numMessages)
{
	this->sinkLock.lock();
	for (size_t i = 0; i < this->messages.size() && i < numMessages; i++) {
		auto msg = messages.back();
		auto cInst = sdk::ClientInstance::get();
		auto lp = cInst->getLocalPlayer();
		if (lp) {
			lp->displayClientMessage(msg);
		}
	}
	this->sinkLock.unlock();
}

void ClientMessageSink::push(std::string const& message)
{
	this->sinkLock.lock();
	this->messages.push_back(message);
	this->sinkLock.unlock();
}

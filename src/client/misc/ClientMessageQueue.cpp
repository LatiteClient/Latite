#include "pch.h"
#include "ClientMessageQueue.h"
#include "util/Logger.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/client/game/ClientInstance.h"

void ClientMessageQueue::doPrint(int numMessages) {
	lock.lock();
	int msgs = 0;
	for (auto it = messages.begin(); it != messages.end();) {
		if (msgs >= numMessages) break;

		auto& msg = *it;
		auto cInst = SDK::ClientInstance::get();
		auto lp = cInst->getLocalPlayer();
		if (lp) {
			LatiteClientMessageEvent ev{ msg };
			Eventing::get().dispatch(ev);

			lp->displayClientMessage(msg);
			messages.erase(it);
			continue;
		}
		++it;
		msgs++;
	}
	lock.unlock();
}

void ClientMessageQueue::push(std::string const& message) {
	this->lock.lock();
	this->messages.push_back(message);
	this->lock.unlock();
}

void ClientMessageQueue::push(std::wstring const& message) {
	push(util::WStrToStr(message));
}

void ClientMessageQueue::display(std::string const& message) {
	auto cInst = SDK::ClientInstance::get();
	auto lp = cInst->getLocalPlayer();
	if (lp) {
		LatiteClientMessageEvent ev{ message };
		Eventing::get().dispatch(ev);
		lp->displayClientMessage(message);
	}
}

void ClientMessageQueue::display(std::wstring const& message) {
	display(util::WStrToStr(message));
}

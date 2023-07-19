#pragma once
#include "Listenable.h"

class Event {
public:
	[[nodiscard]] bool isCancellable() { return cancellable; }
protected:
	bool cancellable = false;
};

class Cancellable : public Event {
	bool cancel = false;
public:
	Cancellable() {
		cancellable = true;
	}

	void setCancelled(bool b) {
		cancel = b;
	}
};

using EventListenerFunc = void(Listener::*)(Event&);

struct EventListener {
	constexpr EventListener(EventListenerFunc fp, Listener* ptr) : fptr(fp), listener(ptr) {}

	EventListenerFunc fptr;
	class Listener* listener;
};
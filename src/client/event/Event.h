#pragma once
#include "Listener.h"

class Event {
public:
	Event(Event&) = delete;
	Event(Event&&) = delete;
	Event() {}

	[[nodiscard]] bool isCancellable() const { return cancellable; }
protected:
	bool cancellable = false;
private:
};

class Cancellable : public Event {
	bool cancel = false;
public:
	Cancellable() {
		cancellable = true;
	}

	void setCancelled(bool b = true) {
		cancel = b;
	}

	[[nodiscard]] bool isCancelled() const {
		return cancel;
	}
};

using EventListenerFunc = void(Listener::*)(Event&);

struct EventListener {
	constexpr EventListener(EventListenerFunc fp, Listener* ptr, bool callWhileInactive, int priority) : fptr(fp), listener(ptr), callWhileInactive(callWhileInactive), priority(priority) {}

	EventListenerFunc fptr;
	class Listener* listener;
	int priority = 0;
	bool callWhileInactive;
};
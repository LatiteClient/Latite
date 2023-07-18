#pragma once
#include "api/feature/module/Module.h"

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

class IModule;
using EventListenerFunc = void(IModule::*)(Event&);

struct EventListener {
	EventListener(EventListenerFunc fp, IModule* ptr) : fptr(fp), feature(ptr) {}

	EventListenerFunc fptr;
	class IModule* feature;
};
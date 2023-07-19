#pragma once
#include "api/manager/Manager.h"
#include "api/feature/module/Module.h"
#include "Listenable.h"
#include "Event.h"

class IEventManager {
public:
	template <typename T>
	void dispatchEvent(T& ev) {
		static_assert(std::is_convertible<T*, Event*>::value, "type is not an Event");
		for (auto& pair : listeners) {
			if (pair.first == T::hash) {
				if (pair.second.listener->shouldListen()) {
					(pair.second.listener->*pair.second.fptr)(ev);
				}
			}
		}
	}

	template <typename T>
	void listen(Listener* ptr, EventListenerFunc listener) {
		static_assert(std::is_convertible<T*, Event*>::value, "type is not an Event");
		listeners.push_back({ T::hash, EventListener{ listener, ptr } });
	}

	//virtual void init() = 0;
protected:
	std::vector<std::pair<uint32_t, EventListener>> listeners;
};

#pragma once
#include "api/manager/Manager.h"
#include "api/feature/module/Module.h"
#include "Listenable.h"
#include "Event.h"
#include <algorithm>

class IEventManager {
public:
	template <typename T>
	bool dispatchEvent(T& ev) {
		static_assert(std::is_convertible<T*, Event*>::value, "type must inherit from Event");
		//static_assert(!(std::is_convertible<Event*, T*>::value), "type must not be an Event");

		mutex.lock();
		std::sort(listeners.begin(), listeners.end(), [](std::pair<uint32_t, EventListener> const& left,
			std::pair<uint32_t, EventListener> const& right) {
				return left.second.priority > right.second.priority;
			});

		for (auto& pair : listeners) {
			if (pair.first == T::hash) {
				if (pair.second.listener->shouldListen() || pair.second.callWhileInactive) {
					auto isCancel = ev.isCancellable();
					(pair.second.listener->*pair.second.fptr)(ev);
					if (isCancel) {
						auto& cEv = reinterpret_cast<Cancellable&>(ev);
						if (cEv.isCancelled()) {
							mutex.unlock();
							return true;
						}
					}
				}
			}
		}
		mutex.unlock();
		return false;
	}

	template <typename T>
	void listen(Listener* ptr, EventListenerFunc listener, int priority = 0, bool callWhileInactive = false) {
		mutex.lock();
		static_assert(std::is_convertible<T*, Event*>::value, "type is not an Event");
		listeners.push_back({ T::hash, EventListener{ listener, ptr, callWhileInactive, priority } });
		mutex.unlock();
	}

	void unlisten(Listener* ptr) {
		mutex.lock();
		for (auto it = listeners.begin(); it != listeners.end();) {
			if (it->second.listener == ptr) {
				listeners.erase(it);
				continue;
			}
			++it;
		}
		mutex.unlock();
	}

	//virtual void init() = 0;
protected:
	std::mutex mutex;
	std::vector<std::pair<uint32_t, EventListener>> listeners;
};

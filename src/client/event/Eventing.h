#pragma once
#include "client/event/Event.h"
#include "client/event/Listener.h"
#include <shared_mutex>
#include <vector>
#include <algorithm>
#include <type_traits>

class Eventing final {
public:
	Eventing() = default;
	~Eventing() = default;

    template <typename T>
    bool dispatch(T&& ev) requires std::derived_from<std::remove_reference_t<T>, Event> {
        using EventType = std::remove_reference_t<T>;
        std::shared_lock lock{ mutex };

        for (auto& pair : listeners) {
            if (pair.first == EventType::hash) {
                if (pair.second.listener->shouldListen() || pair.second.callWhileInactive) {
                    auto isCancel = ev.isCancellable();
                    (pair.second.listener->*pair.second.fptr)(ev);
                    if (isCancel) {
                        auto& cEv = reinterpret_cast<Cancellable&>(ev);
                        if (cEv.isCancelled()) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    // DO NOT USE, use listen<Event, &Listener::func> instead
    template <typename T>
    void listen(Listener* ptr, EventListenerFunc listener, int priority = 0, bool callWhileInactive = false) requires std::derived_from<T, Event> {
        std::shared_lock lock{ mutex };
        listeners.push_back({ T::hash, EventListener{ listener, ptr, callWhileInactive, priority } });
        std::sort(listeners.begin(), listeners.end(), [](std::pair<uint32_t, EventListener> const& left,
                                                         std::pair<uint32_t, EventListener> const& right) {
            return left.second.priority > right.second.priority;
        });
    }

    template <typename T, auto listener>
    void listen(Listener* ptr, int priority = 0, bool callWhileInactive = false) requires std::derived_from<T, Event> {
        static_assert(sizeof(listener) == 16, "Unsupported listener function type");
        struct MFPtr {
            EventListenerFunc ptr;
            ptrdiff_t adj;
        };

        MFPtr mfp = std::bit_cast<MFPtr>(listener);

        std::shared_lock lock{ mutex };
        listeners.push_back({ T::hash, EventListener{ mfp.ptr, ptr, callWhileInactive, priority } });
        std::sort(listeners.begin(), listeners.end(), [](std::pair<uint32_t, EventListener> const& left,
                                                         std::pair<uint32_t, EventListener> const& right) {
            return left.second.priority > right.second.priority;
        });
    }

    void unlisten(Listener* ptr) {
        std::shared_lock lock{ mutex };
        for (auto it = listeners.begin(); it != listeners.end();) {
            if (it->second.listener == ptr) {
                listeners.erase(it);
                continue;
            }
            ++it;
        }
    }

	// Substitute for Latite::getEventing
	[[nodiscard]] static Eventing& get();
private:
    std::shared_mutex mutex;
    std::vector<std::pair<uint32_t, EventListener>> listeners;
};
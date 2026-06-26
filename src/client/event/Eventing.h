#pragma once
#include "client/event/Event.h"
#include "client/event/Listener.h"
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <shared_mutex>
#include <vector>
#include <algorithm>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>

class Eventing final {
public:
    Eventing() = default;
    ~Eventing() = default;

    template<typename T>
    bool dispatch(T&& ev)
        requires std::derived_from<std::remove_reference_t<T>, Event>
    {
        using EventType = std::remove_reference_t<T>;
        std::vector<Registration> snapshot;

        {
            std::shared_lock lock { mutex };
            for (auto const& registration : listeners) {
                if (registration.hash == EventType::hash) {
                    snapshot.push_back(registration);
                }
            }
        }

        for (auto const& registration : snapshot) {
            auto const& state = registration.state;
            std::lock_guard invocationLock { state->invocationMutex };

            if (!state->active || !state->listener) {
                continue;
            }

            if (registration.callback.callWhileInactive || state->listener->shouldListen()) {
                const bool isCancel = ev.isCancellable();
                (state->listener->*registration.callback.fptr)(ev);
                if (isCancel) {
                    auto& cEv = reinterpret_cast<Cancellable&>(ev);
                    if (cEv.isCancelled()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // DO NOT USE, use listen<Event, &Listener::func> instead
    template<typename T>
    void listen(Listener* ptr, EventListenerFunc listener, int priority = 0, bool callWhileInactive = false)
        requires std::derived_from<T, Event>
    {
        addListener(T::hash, EventListener { listener, ptr, callWhileInactive, priority });
    }

    template<typename T, auto listener>
    void listen(Listener* ptr, int priority = 0, bool callWhileInactive = false)
        requires std::derived_from<T, Event>
    {
        struct MFPtr {
            const EventListenerFunc ptr;
            const ptrdiff_t adj;
        };

        if constexpr (sizeof(listener) == sizeof(EventListenerFunc)) {
            addListener(T::hash,
                        EventListener { static_cast<EventListenerFunc>(listener), ptr, callWhileInactive, priority });
        } else if constexpr (sizeof(listener) == sizeof(MFPtr)) {
            const MFPtr mfp = std::bit_cast<MFPtr>(listener);

            addListener(T::hash, EventListener { mfp.ptr, ptr, callWhileInactive, priority });
        } else {
            static_assert(false, "Unsupported listener function type");
        }
    }

    void unlisten(Listener* ptr) {
        if (!ptr) {
            return;
        }

        std::shared_ptr<ListenerState> state;
        {
            std::unique_lock lock { mutex };
            auto stateIt = listenerStates.find(ptr);
            if (stateIt == listenerStates.end()) {
                return;
            }

            state = std::move(stateIt->second);
            listenerStates.erase(stateIt);
            std::erase_if(listeners, [&](Registration const& registration) {
                return registration.state == state;
            });
        }

        std::lock_guard invocationLock { state->invocationMutex };
        state->active = false;
        state->listener = nullptr;
    }

    // Substitute for Latite::getEventing
    [[nodiscard]] static Eventing& get();

private:
    struct ListenerState {
        explicit ListenerState(Listener* listener)
            : listener(listener) {}

        std::recursive_mutex invocationMutex;
        Listener* listener;
        bool active = true;
    };

    struct Registration {
        uint32_t hash;
        EventListener callback;
        std::shared_ptr<ListenerState> state;
    };

    void addListener(uint32_t hash, EventListener callback) {
        if (!callback.listener || callback.fptr == nullptr) {
            return;
        }

        std::unique_lock lock { mutex };
        auto [stateIt, inserted] = listenerStates.try_emplace(callback.listener);
        if (inserted) {
            stateIt->second = std::make_shared<ListenerState>(callback.listener);
        }

        listeners.push_back({ hash, callback, stateIt->second });
        std::stable_sort(listeners.begin(), listeners.end(), [](Registration const& left, Registration const& right) {
            return left.callback.priority > right.callback.priority;
        });
    }

    std::shared_mutex mutex;
    std::vector<Registration> listeners;
    std::unordered_map<Listener*, std::shared_ptr<ListenerState>> listenerStates;
};

#ifndef CODEX_EVENT_DISPATCHER_EVENT_DISPATCHER_HPP
#define CODEX_EVENT_DISPATCHER_EVENT_DISPATCHER_HPP

#include <concepts>
#include <cstdio>
#include <functional>
#include <vector>

namespace codex {

/** Base class tag to identify events. */
struct BaseEvent {};
/** Sample button click event. */
struct OnClick : BaseEvent { int button; };
/** Sample key press event. */
struct OnKey : BaseEvent { int key; };

/** Semi-arbitrary method to identify an event type. */
template<typename T> concept Event = std::derived_from<T, BaseEvent>;
/** Constrains a callable to an event type. */
template<typename T, typename E>
concept EventListener = Event<E> && std::invocable<T, E>;

namespace detail {

/** Storage base class for a given event type. */
template<Event E> struct EventListenerVec
    { protected: std::vector<std::function<void(E)>> v; };

}

/**
 * A type-safe event listener/dispatcher.
 * Event listeners for a particular event type can be added via \ref
 * add_listener.  Subsequent calls to \ref trigger will invoke each listener for
 * that event type to process it.
 * \tparam Es Types of events supported by the dispatcher.
 */
template<Event ...Es>
class EventDispatcher : detail::EventListenerVec<Es>... {
public:
    /** Adds a callback for a given event type. */
    template<Event E, EventListener<E> L>
    void add_listener(L l) { this->vec<E>().emplace_back(std::move(l)); }
    /** Invokes all listeners associated with the event type. */
    template<Event E> void trigger(E e) { for(auto &x: this->vec<E>()) x(e); }
private:
    /** Helper method to access the storage for a particular event type. */
    template<Event E>
    requires std::is_base_of_v<
        detail::EventListenerVec<E>, EventDispatcher<Es...>>
    decltype(auto) vec() { return (detail::EventListenerVec<E>::v); }
};

}

#endif

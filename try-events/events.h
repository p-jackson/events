#ifndef EVENTS_HEADER_INCLUDED
#define EVENTS_HEADER_INCLUDED

#include <cstddef>
#include <functional>
#include <type_traits>
#include <vector>

namespace events {

using EventName = unsigned int;

namespace detail {

inline constexpr EventName calcEventName(const char* s, EventName l) {
  return !l ? 0u : (s[0] << (8 * (l - 1))) + calcEventName(s + 1, l - 1);
}

template<EventName, class... Decls>
struct FindListener { using type = void; };

template<EventName name, class Decl, class... Decls>
struct FindListener<name, Decl, Decls...> {
  using A = std::integral_constant<EventName, name>;
  using B = std::integral_constant<EventName, Decl::name>;
  using type = std::conditional_t<std::is_same<A, B>::value,
    typename Decl::type,
    typename FindListener<name, Decls...>::type
  >;
};

template<class... Declarations>
class EventStore {
public:
  void add() {}
  void get() {}
};

template<class Decl, class... Decls>
class EventStore<Decl, Decls...> : public EventStore<Decls...> {

  using Tag = std::integral_constant<EventName, Decl::name>;
  using Listener = typename Decl::type;

  std::vector<Listener> listeners;

public:
  using EventStore<Decls...>::add;
  using EventStore<Decls...>::get;

  void add(Tag, Listener listener) {
    listeners.push_back(std::move(listener));
  }

  std::vector<Listener>& get(Tag) {
    return listeners;
  }
};

} // namespace detail

namespace literals {

constexpr EventName operator"" _e(const char* s, std::size_t len) {
  return detail::calcEventName(s, static_cast<unsigned int>(len));
}

} // namespace literals

template<class T, class... Declarations>
class EventEmitter : private detail::EventStore<Declarations...> {

  template<EventName n>
  using ListenerType = typename detail::FindListener<n, Declarations...>::type;

public:
  template<EventName name>
  T& on(ListenerType<name> listener) {
    using Tag = std::integral_constant<EventName, name>;
    add(Tag{}, std::move(listener));
    return *static_cast<T*>(this);
  }

  template<EventName name, typename... Args>
  T& addListener(Args&&... args) {
    return on<name>(std::forward<Args>(args)...);
  }

  template<EventName name, typename... Args>
  void trigger(Args&&... args) {
    using Tag = std::integral_constant<EventName, name>;
    for (auto& listener : get(Tag{}))
      listener(std::forward<Args>(args)...);
  }
};

template<EventName name_, typename Signature>
struct Declaration {
  static constexpr EventName name = name_;
  using type = std::function<Signature>;
};

} // namespace events

#endif
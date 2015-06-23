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
struct FindCBType { using type = void; };

template<EventName name, class Decl, class... Decls>
struct FindCBType<name, Decl, Decls...> {
  using A = std::integral_constant<EventName, name>;
  using B = std::integral_constant<EventName, Decl::name>;
  using type = std::conditional_t<std::is_same<A, B>::value,
    typename Decl::type,
    typename FindCBType<name, Decls...>::type
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
  using Callback = typename Decl::type;

  std::vector<Callback> callbacks;

public:
  using EventStore<Decls...>::add;
  using EventStore<Decls...>::get;

  void add(Tag, Callback callback) {
    callbacks.push_back(std::move(callback));
  }

  std::vector<Callback>& get(Tag) {
    return callbacks;
  }
};

} // namespace detail

namespace literals {

constexpr EventName operator"" _e(const char* s, std::size_t len) {
  return detail::calcEventName(s, static_cast<unsigned int>(len));
}

} // namespace literals

template<class... Declarations>
class EventEmitter : private detail::EventStore<Declarations...> {

  template<EventName n>
  using CallbackType = typename detail::FindCBType<n, Declarations...>::type;

public:
  template<EventName name>
  void on(CallbackType<name> callback) {
    using Tag = std::integral_constant<EventName, name>;
    add(Tag{}, std::move(callback));
  }

  template<EventName name, typename... Args>
  void trigger(Args... args) {
    using Tag = std::integral_constant<EventName, name>;
    for (auto& callback : get(Tag{}))
      callback(std::forward<Args>(args)...);
  }
};

template<EventName name_, typename Signature>
struct Declaration {
  static constexpr EventName name = name_;
  using type = std::function<Signature>;
};

} // namespace events

#endif
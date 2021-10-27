//
// Copyright (c) 2021 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <string_view>
#include <algorithm>
#include <utility>

template <auto Size>
struct fixed_string {
  char data[Size + 1]{};
  static constexpr auto size = Size;

  constexpr explicit(false) fixed_string(char const* str) {
    std::copy_n(str, Size + 1, data);
  }
  constexpr explicit(false) operator std::string_view() const {
    return {data, Size};
  }
};
template <auto Size>
fixed_string(char const (&)[Size]) -> fixed_string<Size - 1>;

template <fixed_string Name, class TValue>
struct arg {
  static constexpr auto name = Name;
  TValue value{};
  template <class T>
  constexpr auto operator=(const T& t) {
    return arg<Name, T>{.value = t};
  }
};

template <fixed_string Name>
constexpr auto operator""_t() {
  return arg<Name, int>{};
}

namespace nt {
template <fixed_string Name, class... Ts>
struct namedtuple : Ts... {
  static constexpr auto name_v = Name;
  static constexpr auto size = sizeof...(Ts);

  constexpr explicit(true) namedtuple(Ts... ts) : Ts{ts}... {}
  constexpr explicit(true) namedtuple(auto... ts) : Ts{ts}... {}

  template<fixed_string>
  constexpr auto get(...) -> void { }

  template<fixed_string Name_, class TValue>
  constexpr decltype(auto) get(arg<Name_, TValue>* t) {
    return (t->value);
  }

  template <class T>
  constexpr auto& operator[](const T) {
    return get<T::name>(this);
  }

  auto& assign(auto&&... ts) {
    if constexpr ((requires {
                    ts.name;
                    ts.value;
                  } and
                   ...)) {
      ((static_cast<decltype(ts)&>(*this) = ts), ...);
    } else {
      ((static_cast<Ts&>(*this).value = ts), ...);
    }
    return *this;
  }
};
template <class... Ts>
namedtuple(Ts...) -> namedtuple<"", Ts...>;
}  // namespace nt

template <fixed_string Name = "", class... Ts>
constexpr auto namedtuple(Ts... ts) {
  return nt::namedtuple<Name, Ts...>(ts...);
}

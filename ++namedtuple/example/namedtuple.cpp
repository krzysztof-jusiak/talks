//
// Copyright (c) 2021 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <https://raw.githubusercontent.com/boost-experimental/ut/master/include/boost/ut.hpp>
#include <algorithm>
#include <type_traits>
#include <any>

template<auto Size>
struct fixed_string {
  char data[Size + 1]{};
  static constexpr auto size = Size;
  constexpr explicit(false) fixed_string(char const* str) { std::copy_n(str, Size + 1, data); }
  constexpr explicit(false) operator std::string_view() const { return {data, Size}; }
};
template<auto Size> fixed_string(char const (&)[Size]) -> fixed_string<Size - 1>;

namespace detail {
template <class TDefault, fixed_string, template <fixed_string, class> class>
auto map_lookup(...) -> TDefault;
template <class, fixed_string TKey, template <fixed_string, class> class TPair, class TValue>
auto map_lookup(TPair<TKey, TValue> *) -> TPair<TKey, TValue>;
} // namespace detail

template <class T, fixed_string TKey, class TDefault = void, template <fixed_string, class> class TPair = std::pair>
using map_lookup = decltype(detail::map_lookup<TDefault, TKey, TPair>(static_cast<T *>(nullptr)));

template<class... Ts> struct inherit : Ts... {};


template<class T>
struct any_get : T {
  using T::T;

  template<class U>
  constexpr U get() const {
    return std::any_cast<U>(*this);
  }
};

template<fixed_string Str, class TValue = any_get<std::any>>
struct arg {
  static constexpr auto str = Str;
  TValue value{};
  template<class T> constexpr auto operator=(const T& t) { return arg<Str, T>{.value = t}; }
};

template<fixed_string Str> constexpr auto operator""_t() { return arg<Str, any_get<std::any>>{}; }

template<class... Ts>
struct namedtuple : std::tuple<Ts...> {
  using std::tuple<Ts...>::tuple;

  template<class T> constexpr const auto& operator[](const T) const requires (not std::is_void_v<map_lookup<inherit<Ts...>, T::str, void, arg>>) {
    using type = map_lookup<inherit<Ts...>, T::str, void, arg>;
    return std::get<type>(*this).value;
  }

  template<class T> constexpr auto& operator[](const T) requires (not std::is_void_v<map_lookup<inherit<Ts...>, T::str, void, arg>>) {
    using type = map_lookup<inherit<Ts...>, T::str, void, arg>;
    return std::get<type>(*this).value;
  }
};
template<class... Ts> namedtuple(Ts&&...) -> namedtuple<Ts...>;

int main() {
  using namespace boost::ut;

  "named tuple"_test = [] {
    should("allow empty") = [] {
      const auto nt = namedtuple{};
      expect(not [](auto t) { return requires { t[""_t]; }; }(nt));
    };

    should("support direct initialization") = [] {
      const auto nt = namedtuple{"price"_t = 42, "size"_t = 100};
      expect([](auto t) { return requires { t["price"_t]; }; }(nt));
      expect([](auto t) { return requires { t["size"_t]; }; }(nt));
      expect(not [](auto t) { return requires { t["quantity"_t]; }; }(nt));
      expect(42_i == nt["price"_t] and 100_i == nt["size"_t]);
    };

    should("support modification") = [] {
      auto nt = namedtuple{"price"_t = int{}, "size"_t = std::size_t{}};
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == nt["price"_t] and 34_u == nt["size"_t]);
    };

    should("support any type") = [] {
      auto nt = namedtuple{"price"_t, "size"_t};
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == nt["price"_t].get<int>() and 34_u == nt["size"_t].get<unsigned int>());
    };
  };
}

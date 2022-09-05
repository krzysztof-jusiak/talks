//
// Copyright (c) 2021 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/ut.hpp>
#include <algorithm>
#include <any>
#include <experimental/iterator>
#include <iostream>
#include <iterator>
#include <string_view>
#include <type_traits>
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
  constexpr auto dt() const { return data; }
};
template <auto Size>
fixed_string(char const (&)[Size]) -> fixed_string<Size - 1>;

namespace detail {
template <class TDefault, fixed_string, template <fixed_string, class> class>
auto map_lookup(...) -> TDefault;
template <class, fixed_string TKey, template <fixed_string, class> class TArg,
          class TValue>
auto map_lookup(TArg<TKey, TValue>*) -> TArg<TKey, TValue>;

template <class TDefault, class, template <class, class> class>
auto map_lookup(...) -> TDefault;
template <class, class TKey, template <class, class> class TArg, class TValue>
auto map_lookup(TArg<TKey, TValue>*) -> TArg<TKey, TValue>;
}  // namespace detail

template <class T, fixed_string TKey, class TDefault,
          template <fixed_string, class> class TArg>
using map_lookup = decltype(detail::map_lookup<TDefault, TKey, TArg>(
    static_cast<T*>(nullptr)));

struct any : std::any {
  any() = default;
  template <class T>
  explicit(false) any(const T& a)
      : std::any{a},
        print{[](std::ostream& os, const std::any& a) -> std::ostream& {
          if constexpr (requires { os << std::any_cast<T>(a); }) {
            os << std::any_cast<T>(a);
          } else if constexpr (requires {
                                 std::begin(std::any_cast<T>(a));
                                 std::end(std::any_cast<T>(a));
                               }) {
            auto obj = std::any_cast<T>(a);
            std::copy(std::begin(obj), std::end(obj),
                      std::experimental::make_ostream_joiner(os, ','));
          } else {
            os << a.type().name();
          }
          return os;
        }} {}
  template <class T>
  constexpr explicit(false) operator T() const {
    return std::any_cast<T>(*this);
  }

  friend std::ostream& operator<<(std::ostream& os, const any& a) {
    return a.print(os, a);
  }

 private:
  std::ostream& (*print)(std::ostream&, const std::any&){};
};

template <fixed_string Str, class TValue>
struct arg {
  using value_type = TValue;
  static constexpr auto str = Str;
  TValue value{};
  template <class T>
  constexpr auto operator=(const T& t) {
    return arg<Str, T>{.value = t};
  }
};

template <fixed_string Str>
constexpr auto operator""_t() {
  return arg<Str, any>{};
}

template <class... Ts>
struct inherit : Ts... {};

namespace nt {
template <fixed_string Name, class... Ts>
struct named_tuple : Ts... {
  constexpr explicit(true) named_tuple(Ts... ts) : Ts{ts}... {}
  constexpr named_tuple(decltype(Name), Ts... ts) : Ts{ts}... {}

  template <class T, class TArg = map_lookup<named_tuple, T::str, void, arg>>
  constexpr const auto& operator[](const T) const
      requires(not std::is_void_v<TArg>) {
    return static_cast<const TArg&>(*this).value;
  }

  template <class T, class TArg = map_lookup<named_tuple, T::str, void, arg>>
  constexpr auto& operator[](const T) requires(not std::is_void_v<TArg>) {
    return static_cast<TArg&>(*this).value;
  }

  auto& assign(auto&&... ts) {
    if constexpr ((requires {
                    ts.str;
                    ts.value;
                  } and
                   ...)) {
      ((static_cast<decltype(ts)&>(*this) = ts), ...);
    } else {
      ((static_cast<Ts&>(*this).value = ts), ...);
    }
    return *this;
  }

  template <std::size_t N>
  auto& get() {
    auto id_type = []<auto... Ns>(std::index_sequence<Ns...>) {
      return inherit<
          std::pair<std::integral_constant<std::size_t, Ns>, Ts>...>{};
    }
    (std::make_index_sequence<sizeof...(Ts)>{});
    return static_cast<
               typename decltype(detail::map_lookup<
                                 void, std::integral_constant<std::size_t, N>,
                                 std::pair>(&id_type))::second_type&>(*this)
        .value;
  }

  friend std::ostream& operator<<(std::ostream& os, const named_tuple& nt) {
    os << std::string_view{Name} << '{';
    [&]<auto... Ns>(std::index_sequence<Ns...>) {
      ((os << (Ns ? "," : "") << std::string_view{Ts::str} << ':'
           << static_cast<const map_lookup<named_tuple, Ts::str, void, arg>&>(
                  nt)
                  .value),
       ...);
    }
    (std::make_index_sequence<sizeof...(Ts)>{});
    os << '}';
    return os;
  }
};
template <class... Ts>
named_tuple(Ts...) -> named_tuple<"", Ts...>;
}  // namespace nt

template <fixed_string Name = "", class... Ts>
constexpr auto named_tuple(Ts... ts) {
  return nt::named_tuple<Name, Ts...>(ts...);
}

namespace std {
template <fixed_string Name, class... Ts>
struct tuple_size<nt::named_tuple<Name, Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <std::size_t N, fixed_string Name, class... Ts>
struct tuple_element<N, nt::named_tuple<Name, Ts...>> {
  using type =
      decltype(std::declval<nt::named_tuple<Name, Ts...>>().template get<N>());
};
}  // namespace std

int main() {
  using namespace boost::ut;

  "named tuple"_test = [] {
    should("allow empty") = [] {
      const auto nt = named_tuple();
      expect(not [](auto t) {
        return requires { t[""_t]; };
      }(nt));
    };

    should("support direct initialization") = [] {
      const auto nt = named_tuple<"Trade">("price"_t = 42, "size"_t = 100);
      std::cout << nt << std::endl;
      expect([](auto t) {
        return requires { t["price"_t]; };
      }(nt));
      expect([](auto t) {
        return requires { t["size"_t]; };
      }(nt));
      expect(not [](auto t) {
        return requires { t["quantity"_t]; };
      }(nt));
      expect(42_i == nt["price"_t] and 100_i == nt["size"_t]);
    };

    should("support assignment") = [] {
      auto nt = named_tuple("price"_t = int{}, "size"_t = std::size_t{});
      expect(0_i == nt["price"_t] and 0_ul == nt["size"_t]);

      nt.assign(42, 99u);
      expect(42_i == nt["price"_t] and 99_ul == nt["size"_t]);

      nt.assign("price"_t = 11, "size"_t = 1234ul);
      expect(11_i == nt["price"_t] and 1234_ul == nt["size"_t]);
    };

    should("support modification") = [] {
      auto nt = named_tuple("price"_t = int{}, "size"_t = std::size_t{});
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == nt["price"_t] and 34_u == nt["size"_t]);
    };

    should("support any type") = [] {
      auto nt = named_tuple("price"_t, "size"_t);
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == int(nt["price"_t]) and 34_u == unsigned(nt["size"_t]));
    };

    should("support composition") = [] {
      auto n1 = named_tuple("quantity"_t = 42);
      auto n2 = named_tuple("value"_t = 100u);
      auto nt = named_tuple<"Msg">(n1, "price"_t, "size"_t, n2);
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == int(nt["price"_t]) and 34_u == unsigned(nt["size"_t]) and
             42_i == nt["quantity"_t]);
      std::cout << n1 << std::endl << n2 << std::endl << nt << std::endl;
    };

    should("get by value") = [] {
      auto nt = named_tuple("price"_t = 100, "size"_t = 42u);
      expect(100_i == nt.get<0>() and 42_u == nt.get<1>());
    };

    should("support decomposition") = [] {
      auto nt = named_tuple("price"_t = 100, "size"_t = 42u);
      auto [price, size] = nt;
      expect(100_i == price and 42_u == size);
    };

    should("pack the tuple") = [] {
      auto nt = named_tuple("_1"_t = char{}, "_2"_t = int{}, "_3"_t = char{});
      expect(constant<12_u == sizeof(nt)>);
    };

    should("support arrays") = [] {
      auto nt = named_tuple<"Person">("name"_t = std::string{}, "children"_t);
      nt.assign("name"_t = std::string{"John"},
                "children"_t = any(std::array{"Mike", "Keke"}));
      std::cout << nt << std::endl;

      nt.assign("Mike", std::array{"John"});
      std::cout << nt << std::endl;
    };
  };
}

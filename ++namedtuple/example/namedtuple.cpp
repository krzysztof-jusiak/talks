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
#include <tuple>
#include <vector>

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

using std::literals::string_view_literals::operator""sv;

static_assert(""sv == fixed_string(""));
static_assert("name"sv == fixed_string("name"));

template <fixed_string Name, class TValue>
struct arg {
  static constexpr auto name = Name;
  TValue value{};
  template <class T>
  constexpr auto operator=(const T& t) {
    return arg<Name, T>{.value = t};
  }
};

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

template <class... Ts> struct inherit : Ts... {};

static_assert(std::is_same_v<void, map_lookup<inherit<arg<"price", double>, arg<"size", int>>, "unknown", void, arg>>);
static_assert(std::is_same_v<arg<"price", double>, map_lookup<inherit<arg<"price", double>, arg<"size", int>>, "price", void, arg>>);
static_assert(std::is_same_v<arg<"size", int>, map_lookup<inherit<arg<"price", double>, arg<"size", int>>, "size", void, arg>>);

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

template <fixed_string Name>
constexpr auto operator""_t() {
  return arg<Name, any>{};
}

//static_assert("name"sv == ("name"_t = 42).name);
//static_assert(42       == ("name"_t = 42).value);

namespace nt {
template <fixed_string Name, class... Ts>
struct namedtuple : Ts... {
  static constexpr auto name_v = Name;
  static constexpr auto size = sizeof...(Ts);

  constexpr explicit(true) namedtuple(Ts... ts) : Ts{ts}... {}
  constexpr explicit(true) namedtuple(auto... ts) : Ts{ts}... {}

  template <class T, class TArg = map_lookup<namedtuple, T::name, void, arg>>
  constexpr const auto& operator[](const T) const
      requires(not std::is_void_v<TArg>) {
    return static_cast<const TArg&>(*this).value;
  }

  template <class T, class TArg = map_lookup<namedtuple, T::name, void, arg>>
  constexpr auto& operator[](const T) requires(not std::is_void_v<TArg>) {
    return static_cast<TArg&>(*this).value;
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
                                 std::pair>(&id_type))::second_type&>(*this);
  }

  template <std::size_t N>
  const auto& get() const {
    auto id_type = []<auto... Ns>(std::index_sequence<Ns...>) {
      return inherit<
          std::pair<std::integral_constant<std::size_t, Ns>, Ts>...>{};
    }
    (std::make_index_sequence<sizeof...(Ts)>{});
    return static_cast<
               const typename decltype(detail::map_lookup<
                                 void, std::integral_constant<std::size_t, N>,
                                 std::pair>(&id_type))::second_type&>(*this);
  }

  friend std::ostream& operator<<(std::ostream& os, const namedtuple& nt) {
    os << std::string_view{Name} << '{';
    [&]<auto... Ns>(std::index_sequence<Ns...>) {
      ((os << (Ns ? "," : "") << std::string_view{Ts::name} << ':'
           << static_cast<const map_lookup<namedtuple, Ts::name, void, arg>&>(
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
namedtuple(Ts...) -> namedtuple<"", Ts...>;
}  // namespace nt

template <fixed_string Name = "", class... Ts>
constexpr auto namedtuple(Ts... ts) {
  return nt::namedtuple<Name, Ts...>(ts...);
}

namespace std {
template <fixed_string Name, class... Ts>
struct tuple_size<nt::namedtuple<Name, Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <std::size_t N, fixed_string Name, class... Ts>
struct tuple_element<N, nt::namedtuple<Name, Ts...>> {
  using type =
      decltype(std::declval<nt::namedtuple<Name, Ts...>>().template get<N>());
};

template<std::size_t N, fixed_string Name, class... Ts>
const auto& get(nt::namedtuple<Name, Ts...>&& nt) noexcept {
  return nt.template get<N>();
}
}  // namespace std

template<class T, fixed_string... Names>
concept extends = (requires(T t) { t[arg<Names, any>{}]; } and ...);

int main() {
  using namespace boost::ut;

  "named tuple"_test = [] {
    should("allow empty") = [] {
      const auto nt = namedtuple();
      expect(not [](auto t) {
        return requires { t[""_t]; };
      }(nt));
    };

    should("support direct initialization") = [] {
      const auto nt = namedtuple<"Trade">("price"_t = 42, "size"_t = 100);
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

    should("support initialization") = [] {
      using record_t = nt::namedtuple<"", arg<"price", int>, arg<"size", int>>;
      auto record1 = record_t{
        "price"_t = 42,
        "size"_t = 10
      };
      auto record2 = record_t{
        "price"_t = 43,
        "size"_t = 20
      };

      expect(42_i == record1["price"_t]);
      expect(20_i == record2["size"_t]);
    };

    should("support extends") = [] {
      using record_t = decltype(namedtuple("price"_t = int{}, "size"_t = std::size_t{}));
      auto record = record_t{42, 100ul};
      expect(100_ul == record["size"_t]);

      static_assert(not extends<record_t, "quantity">);
      static_assert(not extends<record_t, "price", "quantity">);
      static_assert(not extends<record_t, "price", "size", "value">);
      static_assert(not extends<record_t, "price", "size", "value">);

      static_assert(extends<record_t, "price", "size">);
      static_assert(extends<record_t, "size", "price">);

      auto empty = namedtuple();
      static_assert(not extends<decltype(empty), "name">);

      auto name = namedtuple(empty, "name"_t = 42);
      static_assert(extends<decltype(name), "name">);

      constexpr auto get_name = [](const extends<"name"> auto& t) {
        return t["name"_t];
      };
      expect(42_i == get_name(name));
    };

    should("support assignment") = [] {
      auto nt = namedtuple("price"_t = int{}, "size"_t = std::size_t{});
      expect(0_i == nt["price"_t] and 0_ul == nt["size"_t]);

      nt.assign(42, 99u);
      expect(42_i == nt["price"_t] and 99_ul == nt["size"_t]);

      nt.assign("price"_t = 11, "size"_t = 1234ul);
      expect(11_i == nt["price"_t] and 1234_ul == nt["size"_t]);
    };

    should("support modification") = [] {
      auto nt = namedtuple("price"_t = int{}, "size"_t = std::size_t{});
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == nt["price"_t] and 34_u == nt["size"_t]);
    };

    should("support any type") = [] {
      auto nt = namedtuple("price"_t, "size"_t);
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == int(nt["price"_t]) and 34_u == unsigned(nt["size"_t]));
    };

    should("support composition") = [] {
      auto n1 = namedtuple("quantity"_t = 42);
      auto n2 = namedtuple("value"_t = 100u);
      auto nt = namedtuple<"Msg">(n1, "price"_t, "size"_t, n2);
      nt["price"_t] = 12;
      nt["size"_t] = 34u;
      expect(12_i == int(nt["price"_t]) and 34_u == unsigned(nt["size"_t]) and
             42_i == nt["quantity"_t]);
      std::cout << n1 << std::endl << n2 << std::endl << nt << std::endl;
    };

    should("support nesting") = [] {
      auto nt1 = namedtuple("first"_t, "last"_t);
      auto nt2 = namedtuple<"Attendee">("name"_t = nt1, "position"_t);
      nt2["name"_t]["first"_t] = "Kris"sv;
      nt2["name"_t]["last"_t] = "Jusiak"sv;
      nt2["position"_t] = "Software Architect"sv;
      std::cout << nt2 << std::endl;
    };

    should("get by value") = [] {
      auto nt = namedtuple("price"_t = 100, "size"_t = 42u);
      expect(100_i == nt.get<0>().value and 42_u == nt.get<1>().value);
    };

    should("support decomposition") = [] {
      auto nt = namedtuple("price"_t = 100, "size"_t = 42u);
      auto [price, size] = nt;
      expect(100_i == price.value and 42_u == size.value);
    };

    should("pack the tuple") = [] {
      auto nt = namedtuple("_1"_t = char{}, "_2"_t = int{}, "_3"_t = char{});
      expect(constant<12_u == sizeof(nt)>);
    };

    should("support arrays") = [] {
      auto nt = namedtuple<"Person">("name"_t = std::string{}, "children"_t);
      nt.assign("name"_t = std::string{"John"},
                "children"_t = any(std::array{"Mike", "Keke"}));
      std::cout << nt << std::endl;

      nt.assign("Mike", std::array{"John"});
      std::cout << nt << std::endl;
    };
  };

  should("apply") = [] {
    auto nt = namedtuple("price"_t = 42, "size"_t = 100);
    auto f = [](const auto&... args) {
      std::cout << '{';
      ((std::cout << "\"" << std::string_view{args.name} << "\" :" << args.value << ','), ...);
      std::cout << '}';
    };
    [&]<auto... Ns>(std::index_sequence<Ns...>) {
      f(nt.template get<Ns>()...);
    }(std::make_index_sequence<decltype(nt)::size>{});
  };

  should("showcase") = [] {
    auto employee = namedtuple<"Employee">("name"_t, "age"_t, "title"_t);

    std::vector<decltype(employee)> employees{};
    employees.emplace_back("John", 22, "Software Engineer");
    employees.emplace_back("Michael", 36, "Senior Software Engineer");

    const auto to_json = [](std::ostream& os, const auto& vs) {
      std::cout << "[{";
      for (const auto& v : vs) {
        os << "\"" << std::string_view{v.name_v} << "\" : {";
        [&]<auto... Ns>(std::index_sequence<Ns...>) {
          ((os << (Ns ? "," : "")
              << "\"" << std::string_view{v.template get<Ns>().name} << "\" : " << v.template get<Ns>().value),
        ...);
        }
        (std::make_index_sequence<std::remove_cvref_t<decltype(v)>::size>{});
        os << "}";
      }
      std::cout << "}]";
    };

    to_json(std::cout, employees);
  };
}

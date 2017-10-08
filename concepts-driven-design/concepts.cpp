#include <type_traits>
struct type{};

template<class... Args, class F>
constexpr auto requires(F&&) {
  return type{};
}

template<class T>
constexpr auto Copyable = requires<T>([](auto a) -> decltype(a++) {});


struct Copyable2 {
  template<class T>
  auto requires() -> std::enable_if_t<
    Copyable<T>
  >;
};

struct _{};

template<auto>
struct concept{};

template<class T = Copyable2(class Blah)>
class Q{};

int main() {
  //bind<Copyable2>.to<Impl>
  //Copyable2 c{42};
}

if constexpr($requires(auto&& c = cond)(c.acquire())) {

if constexpr(__requires([](auto c) -> decltype(c.aquire()))(cond))

if constexpr(__requires<T>(auto c)(c.aquire()))

if constexpr(__requires(_1.acquire(), cond)) {
  cond.acquire();
}
template<class T>
concept bool fff = requires(T a) {
  a.foo();
};

//template<class T>

const auto foo = []( auto &&t) {
  if constexpr(requires(decltype(t) t) { t.foo(); }) {
    return t.foo();
  }
  return 9;
};



int main() {
  struct Foo { int foo() { return 12; } } f;
  return foo(f);
}
template<class T>
concept bool fff = requires(T a) {
  a.foo();
};

//template<class T>

const auto foo = []<class T>( T &&t) {
  if constexpr(requires (T f) { f.foo(); }) {
    return t.foo();
  }
  return 9;
};



int main() {
  struct Foo { int foo() { return 12; } } f;
  return foo(f);
}

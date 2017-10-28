<!-- story how concepts were removed from C++0x  -->

<!-- footer:  kris@jusiak.net | @krisjusiak | linkedin.com/in/kris-jusiak -->

Meeting C++ 2017

# Concepts Driven Design

<hr />

##### Kris Jusiak, Quantlab

---

<!-- footer: Meeting C++ 2017 | Concepts Driven Design | Kris Jusiak -->

# Agenda

<font size="5">
  
* Concepts
  * Motivation
  * History
* Type constraints (C++20)
  * Requirements
    * Design by introspection
  * Named `concepts`
* Concepts emulation (C++17)
* Concepts driven design
  * Static dispatch based on concepts (`CRTP`)
  * Dynamic dispatch (`type erasure`) based on concepts
  * Testing with concepts
* Future of concepts (C++2X)

</font>

---

## Disclaimer

Concepts, although, merged into C++20 draft, are still a subject for future changes.

| Compiler | Version | Notes |
| - | - | - |
| GCC | 6.1 | Requires `-fconcepts` flag |
| MSVC | VS2017 15.5 | - |
| Clang | In progress... | It had C++0x concepts support |

Examples in this talk were compiled using

> `g++7.2 -std=c++2a -fconcepts`

---

<!-- page_number: true -->

# Motivation - Error Novel

```cpp
int main() {
  auto list = std::list{1, 2, 3};
  std::sort(std::begin(list), std::end(list));
}
```

https://godbolt.org/g/RTRgg2

---

# Motivation - Improve error messages

> Without concepts
```cpp
 invalid operands to binary expression 
   ('std::_List_iterator<int>' and 
    'std::_List_iterator<int>')
 std::__lg(__last - __first) * 2);
           ~~~~~~ ^ ~~~~~~~
...many lines lines of output...
```

> With concepts
```cpp
error: cannot call std::sort
 note: concept RandomAccessIterator was not satisfied
since: expression (b-a) will be ill formed
```

---

# Concepts - History

![100%](images/history.png)

> Concepts Lite - https://wg21.link/n3701 
Andrew Sutton, Bjarne Stroustrup, Gabriel Dos Reis

> Concepts TS - https://wg21.link/P0734R0
Andrew Sutton, Bjarne Stroustrup

---

# Concepts - C++20 draft

http://eel.is/c++draft/temp.constr

![100%](images/templates.png)

* Requirements
  * `requires-clause`, `requires-expression`
* Constraints
  * `predicates, conjunctions, disjunctions`
* Named concepts
  * `placeholders`, `abbreviated templates`

---

# Requirements

---

# `Requires-clause`
> Specifies constraints on template arguments or on a function declaration
> `std::enable_if` on steroids

```cpp
template<bool Value>
void foo() requires Value {}
```

```cpp
int main() {
  foo<true>();  // Okay
  foo<false>(); // Error: constraints not satisfied
}
```

> Notes: requires-clause is part of the function signature

---

# `Requires-expression`
> prvalue expression of type bool
  `requires ( [parameters] ) { requirements }		`

```cpp
// type requirement
requires(T) { typename T::value_type; };
```

```cpp
// simple requirement
requires(T t) { t[typename T::value_type{}]; };
```

```cpp
// compound requirement
requires(T t) { { t.empty() } -> bool;  };
```

```cpp
// nested requirement
requires(T t) {
  requires std::is_integral_v<typename T::value_type>;
};
```

---

# `Requires-clause/Requires-expression`

```cpp
template<class T>
constexpr auto foo(T&& x)
  requires requires(T t) { t.bar(); }
{ return x.bar(); }
```

```cpp
  struct { void bar() {} } bar;
  foo(bar); // Okay
  foo(42);  // Error: constraints not satisfied
            //  note: the required expression 
            //        't.bar()' would be ill-formed
```

> Notes: `requires requires`  -> `requires-clause` followed by `requires-expression`

---

# Design by introspection

> Andrei Alexandrescu

[![design_by_introspection](images/design_by_introspection.png)](https://www.youtube.com/watch?v=29h6jGtZD-U)

---

# Design by introspection - D lang

> https://dlang.org/spec/traits.html

```d
auto foo(T)(T x) {
  static if(__traits(hasMember, x, "bar")) {
    return x.bar;
  } else {
    return 0;
  }
}

void main() {
  assert(0 == foo(42));
  
  struct Bar { int bar = 42; }
  Bar bar;
  assert(42 == foo(bar));
}
```

https://godbolt.org/g/wpLXzV

---

# Design by introspection - C++20

```cpp
template<class T>
constexpr auto foo(T x) {
  if constexpr(requires(T t) { t.bar; }) {
    return x.bar;
  } else {
    return 0;
  }
}

int main() {
  assert(0 == foo(42));
  
  struct { int bar{42}; } bar;
  assert(42 == foo(bar));
}
```

https://godbolt.org/g/uot6Bv

---

# Design by introspection - C++

[![100%](images/dbi.png)](https://godbolt.org/g/MFxqWu)

https://godbolt.org/g/MFxqWu

---

# `Named concepts`

> A collection of requirements on a type (variable template)

```cpp
template<template-parameters>
concept concept-name = constraint-expression;
```

> "If you like it then you should have put a name on it", Beyonce rule

---

# `Named concepts`

> Predicate constraints

```cpp
template<class>
concept Always = true; // always satisified
```

```cpp
template<class T>
concept Size32 = sizeof(T) == 4;
```

> Conjunctions, Disjunctions
```cpp
template<class T>
concept SignedIntegral = std::is_integral<T>{} and
                         std::is_signed<T>{};
```

---

# `Named concepts`

> Requirements

```cpp
template<class T>
concept Fooable =           // named concept
  requires(T t) {           // collection of
    typename T::type;       //   type requirement
    { t.foo() } -> void;    //   compound requirement
    requires Movable<T> or  //   nested requirement
             Same<T, int>;  
  };
```

---

# `Named concepts`

> 

> Unconstrained class definition
```cpp
template<class> class Bar {};
```

> Requires expression
```cpp
template<class T> class Bar 
  requires Fooable<T> {}; // requires-expression
```

> Abbreviated templates
```cpp
template<Fooable T>  // Fooable instead of
class Bar {};        // typename/class
```

> Notes: C++17 - Non-type template arguments
```cpp
template<auto T> class Bar {}; // For values -> Bar<42>
```

---

# `Named concepts`

> Example

```cpp
struct tcp_socket { void send(std::string_view); };
struct udp_socket { void send(std::string_view); };
struct file       { void write(std::string_view); };
```

>  Everything Cpp 
>  https://www.youtube.com/watch?v=xsSYPD0v5Mg

---

# `Named concepts`

> Concept definition

```cpp
template<class T> concept Socket = 
  requires(T t, std:string_view data) {
    { t.send(data) } -> void; // compound requirement
};
```

```cpp
template<class T> concept File = 
  requires(T t, std:string_view data) {
    { t.write(data) } -> void; // compound requirement
};
```

---

# `Named concepts`

> Concept overloading

```cpp
template<class T> requires Socket<T>
/*1*/ void forward(T& t, std::string_view data) {
  t.send(data);
}
```

```cpp
template<class T> requires File<T>
/*2*/ void forward(T& t, std::string_view data) {
  t.write(data);
}
```

```cpp
int main() {
 tcp_socket tcp{}; forward(tcp, "tcp data"); // calls 1
 udp_socket udp{}; forward(udp, "udp data"); // calls 1
 file file{}; forward(file, "file data");    // calls 2
}
```

---

# `Named concepts`

> `if constexpr` (C++17)

```cpp
template<class T>
void forward(T& t, std::string_view data) {
  if constexpr(Socket<T>) { // compile-time
    t.send(data);
  } else if constexpr(File<T>) {
    t.write(data);
  }
}
```

> Notes: Branch which is not taken is discarded
> --------- Statement may not compile but syntax has to be valid

---

# `Named concepts`

> `Lambdas`

```cpp
constexpr auto forward = 
  [](auto& t, std::string_view data) {
    using type = std::decay_t<decltype(t)>>;
    if constexpr(Socket<type>) { t.send(data); } else 
    if constexpr(File<type>) { t.write(data); }
  };
```

> `Generic lambdas` (C++17) - https://wg21.link/P0428r2

```cpp
constexpr auto forward = 
  []<class T>(T& t, std::string_view data) { // class T
    if constexpr(Socket<T>) { t.send(data); } else 
    if constexpr(File<T>) { t.write(data); }
  };
```

---

# Placeholders

| Placeholder | Synopsis |
|-|-|
|Unconstrained|`auto`|
|Constrained|`concept-name<[template-argument-list]>`|

```cpp
template<class T> class Foo {};
template<class T> concept Fooable = true;
```

```cpp
// auto - weakest placeholder
auto    foo1 = Foo<int>{};

// C++17 - Constructor Template Argument Deduction
Foo     foo2 = Foo<int>{};

// C++20 - placeholder
Fooable foo3 = Foo<int>{};
```

---

## Requirements in the ISO C++ standard
<center>
  
![150%](images/std_equality_comparable.png)

</center>

##### Haskell typeclasses

```haskell
class EqualityComparable a where
  (==) :: a -> a -> Bool
```

##### C++ concepts
```
template<class T>
concept EqualityComparable = requires(T a, T b) {
  { a == b } -> bool;
};
```

## Concepts - Real life example - Ranges TS

```cpp
template <class I>
concept InputIterator =
  Iterator<I> &&
  Readable<I> &&
  requires(I& i, const I& ci) {
      typename iterator_category_t<I>;
      DerivedFrom<
        iterator_category_t<I>, input_iterator_tag>;
      i++;
  };
```

https://github.com/CaseyCarter/cmcstl2



---

# Concepts emulation (C++17)

---

# Substituation Failure Is Not An Error (SFINAE)

```cpp
template<bool, class = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> { using type = T; };
```

* `false` predicates lead to ill-formed code that's discarded

---

What you can't do with concepts

std::vector<Any> v;
  
v.push_back("Meeting C++");
v.push_back(2017);

---

## [Virtual concepts](https://github.com/andyprowl/virtual-concepts/blob/master/draft/Dynamic%20Generic%20Programming%20with%20Virtual%20Concepts.pdf)

```cpp
template<class T>
concept EqualityComparable = requires(T a, T b) {
  // standard
  { a == b } -> bool;
};
```

```cpp
template<class T>
concept EqualityComparable = requires() {
  // NOT standard!
  auto operator==(const T&, const T&) -> bool;
};
```

---

type erasure

```cpp
template<typename T>
concept Any = requires(T x) {
  requires DefaultConstructible<T> &&
    CopyConstructible<T> &&
    NoThrowMoveConstructible<T> &&
    CopyAssignable<T> &&
    NoThrowMoveAssignable<T> &&
    Destructible<T>;
};
```

```cpp
concept Fooable
```

```cpp
std::vector<virtual Any> v;
v.push_back("Hello"s);
v.push_back(42);
```

---

Shape

draw(virtual Shape);

---

## Syntax

* 1/5

```cpp
template<class T> requires CONCEPT;
void foo(T);
```

* 2/5

```cpp
template<CONCEPT T>
void foo(T);
```

* 3/5

```cpp
void foo(CONCEPT);
```

```cpp
{CONCEPT} void foo(CONCEPT);
```

But

```cpp
template<auto X> // ERROR
```


terse template notation

3/5

---

## Does the Concepts TS Improve on C++17?

https://wg21.link/P0726R0

```cpp
template <class F, class... Args, class =  decltype(
  std::declval<F&&>()(std::declval<Args&&>()...))>
constexpr auto requires_impl(int) { return true; }

template <class F, class... Args>
constexpr auto requires_impl(...) { return false; }

template <class... Args, class F>
constexpr auto requires(F&&) {
  return requires_impl<F&&, Args&&...>(int{});
}
```

---



## Concepts and Metaclasses - https://wg21.link/p0707r0

```cpp
template<class T>
Fooable Foo { };  // static_assert(Fooable<Foo>);
```

---

concepts drive design
is copyable printable etc, can't express that with virutal functions
> Optional interfaces

```cpp
template<class T>
class istream {
public:
  virtual ~istream() noexcept = default;
  virtual void write(T) = 0;
  virtual T read() = 0;
  // virtual void read_complete() = 0; // [optional]
};
```


type erasure with mocking
mocking

---

## Concepts driven design - Goals / Dream

| | |
|-|-|
| Expressiveness | Type constraints for better error messages (Design by Introspection) |
| Loosely coupeled design | Inject all the things! (Policy Design) |
| Performance | Static dispatch by default <br />(based on concepts) |
| Flexiblity | Dynamic dispatch using type erasure (based on the same concepts) |
| Testability | Automatic mocks injection <br />(based on the same concepts) |

---

## Type constraints - [VC](https://github.com/boost-experimental/vc)

#### Non-templated constraints (optional interfaces)

```cpp
struct Readable {               | // Readable Implementation
 template<class T>              |
  auto operator()() const {     | struct Reader { // no inheritance
    MoveConstructible<T> &&    <|> Reader(Reader&&) = default; // âœ”
    MoveAssignable<T> &&       <|> Reader& operator=(Reader&&) = default; // âœ”
    Callable<T, int()>($(read))<|> int read(); // âœ”
  };                     ^      | };
};               ________/      |
                /               | static_assert(
  /* Lambda expression */       |  is_satisfied_by<Readable, Reader>{}
  /* exposing a read() call */  | );
```
<!-- .element: style="margin-left:0%; width:100%" -->

----

## Inject all the things! - [[Boost].DI](https://github.com/boost-experimental/vc)

#### Policy design
```cpp
template<class T = class TException> // `TException` is satisifed by any type
struct ThrowExceptionPolicy {        //  It's like auto in Concepts-lite
  void onError(std::string_view msg) { throw T{msg}; }
};
```
<!-- .element: style="margin-left:0%; width:100%" -->

```cpp
template<class TPolicy = class TErrorPolicy>
class App : TPolicy {
public:
  void run() {
    if (...) { TPolicy::onError("error!"); }
  }
};
```
<!-- .element: style="margin-left:0%; width:100%" -->

```cpp
int main() {
  const auto injector = di::make_injector(                 // wiring
   di::bind<class TException>.to<std::runtime_error>(),    // concept->type
   di::bind<class TErrorPolicy>.to<ThrowExceptionPolicy>() // concept->template
  );
  di::make<App>(injector).run(); // App is a template!
}
```
<!-- .element: style="margin-left:0%; width:100%" -->

----

## [DI](https://github.com/boost-experimental/vc) - 2-Phase resolving (concepts / ctors) 

```cpp
template<class TReader = Readable, // typename = concept
         class TPrinter = Printable>
class App {
  TReader reader;
  TPrinter printer;

public:
  App(TReader reader, TPrinter printer) // constructor
   : reader(reader), printer(printer)   // parameters deduction
  { }

  void run() { printer.print(reader.read()); }
};
```

#### Concepts based injection (compile time wiring)
```cpp
int main() {
 const auto injector = di::make_injector(
   di::bind<Readable>.to<FileReader>(),     // concept -> type
   di::bind<Printable>.to<ConsolePrinter>() // concepts checking
 );                                         // at wiring!
 di::make<App>(injector).run(); // preallocates shared dependencies
```

----

## type_erasure for dynamic dispatch - [VC](https://github.com/boost-experimental/vc)

```cpp
template<class TReader = Readable> // type = concept
class App {
  TReader reader;
  any<Printable> printer; // type erasure based on the same concept
                          // as concepts example
public:
  App(TReader reader, any<Printable> printer) // 100% value semantics
    : reader(reader), printer(printer)
  { }

  void run() { printer.print(reader.read()); }
};
```

#### Dynamic bindings using virtual concepts
```cpp
const auto config = [](std::string_view printer) {
  return di::make_injector(
    di::bind<Readable>.to<FileReader>(),
    di::bind<Printable>([&](auto&& _) {
      return printer == "QT" ?
        _.to<QtPrinter>() : _.to<ConsolePrinter>();
    })
  );
};
```

----

## Automatic / concepts based / mocks injection - [GUnit.GMock](https://github.com/cpp-testing/gunit)

```cpp
"should print read text"_test = [] {
 constexpr auto value = 42;
 auto [app, mocks] = testing::make<App>(); // creates System Under Test
                                           // and Mocks!

 InSequence sequence;
 {
   EXPECT_CALL(mocks<Readable>(), read()).WillOnce(Return(value));
   EXPECT_CALL(mocks<Printable>(), print(value));
 }

 app.run();
};
```
#### It works with concepts/type_erasure and interfaces!


---

## Injecting named concepts - Concepts base design

```cpp
template<class TFoo = Fooable, class TBar = Barable>
class Example { 
public:
  constexpr Example(TFoo, TBar);
};
```

[Boost].DI
```cpp
constexpr auto injector = di::make_injector(
  di::bind<Fooable>.to<Foo>(),
  di::bind<Barable>.to<Bar>()
);
```

```cpp
auto example = injector.create<Example>();
```

---

<!-- page_number: false -->

## Summary

* Simplify enable_if
* can be emulated in C++14

---

```cpp
template <class T>
concept Readable =
  CopyConstructible<T> and
  CopyAssignable<T> and
  requires(T t) {
    { t.read() -> int }
  }
};
```

```cpp
template <class T>
constexpr auto Readable =
  CopyConstructible<T> and
  CopyAssignable<T> and
  Callable<T, int()>($(read)); // expose read for mocking!
```

---

```cpp
Callable<T, int()>($(read)) --> Constraint
          \_  \_____   \___-> name
            \       \
$(name) [](auto t, auto r, auto... args) { // expression
 struct { // inherit from
  auto name(decltype(args)... args) 
    -> decltype(self.name(args...)) { 
   // static polymorphism
   return static_cast<decltype(t) *>(this)
     -> template call_<name, decltype(r)>(args...);
  }
 } _; return _; // local struct
}
```

---

```cpp
template<class... TConstraints>
class gmock_impl : decltype(std::declval<TConstraints::expression>()(
                             TConstraints::args...))... (
public:
 gmock_impl() = default;

 template <class TName, class R, class... TArgs>
 decltype(auto) call_(TArgs &&... args); // calls mocked impl...
};
```

---

template<auto> is something else

Notes:
	* forward(auto, auto);
	* forward(Socket, Socket);

the same concepts requires the same types!!!
but it's not true for auto, auto


Future

```cpp
void foo() requires true {}
void bar() requires false {}
```

```cpp
int main() {
  foo(); // Okay
  bar(); // Error: constraints not satisfied
}
```

> Notes: But C++20 requires-clauses on non-templated functions are ill-formed 

---

---


## Questions?


|   | ![23%](images/bjarne_concepts.jpg)  |
| - | - |
| **Concepts**         | https://wg21.link/P0734R0  | 
| **Virtual Concepts** |  https://github.com/andyprowl/virtual-concepts/blob/master/draft/Dynamic%20Generic%20Programming%20with%20Virtual%20Concepts.pdf | 

<!-- footer:  kris@jusiak.net | @krisjusiak | linkedin.com/in/kris-jusiak -->
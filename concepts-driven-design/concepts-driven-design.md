<!-- footer:  kris@jusiak.net | @krisjusiak | linkedin.com/in/kris-jusiak -->

Meeting C++ 2017

# Concepts Driven Design

<hr />

##### Kris Jusiak

---
<!-- footer: Meeting C++ 2017 | Concepts Driven Design | Kris Jusiak -->

# Agenda

* Problem
* Solution
* Concepts

---

## Design by introspection

```cpp
constexpr auto expr = [] {
  if constexpr(requires(T t) { t.foo(); }) {
    return t.foo();
  }
};
```

---

## Requirements

```cpp
template<class T>
concept Fooable = requires(T t) {
};
```

---

## Concepts

```cpp
auto fooable = Foo{}; // wekeast concept
```

```cpp
Fooable fooable = Foo{};
```

```cpp
Pointer fooable = Foo{};
```

---

## Concepts

```cpp
template<class T>
```

```cpp
template<Fooable T>
```

```cpp
template<auto T> // Error!
```

---


<!-- page_number: true -->

---
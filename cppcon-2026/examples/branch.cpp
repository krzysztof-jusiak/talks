#include <perf/perf.hpp>

[[gnu::optimize("O3")]] const char* fizz_buzz(int n) {
  if (n % 15 == 0) {
    return "FizzBuzz";
  } else if (n % 3 == 0) {
    return "Fizz";
  } else if (n % 5 == 0) {
    return "Buzz";
  } else {
    return "Unknown";
  }
}

int main() {}


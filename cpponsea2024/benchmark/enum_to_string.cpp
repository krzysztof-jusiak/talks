#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include <map>
#include <mph>
#include <unordered_map>
#include <random>
#include <frozen/unordered_map.h>
#include <vector>

enum class permissions : std::uint8_t {
  read     = 1 << 0, // 0b00000001
  write    = 1 << 1, // 0b00000010
  execute  = 1 << 2, // 0b00000100
};

enum color {
  red, green, blue,
};

template <class E> requires std::is_enum_v<E>
constexpr auto enum_to_string(E value) -> std::string_view {
  if (value == color::red)   { return "red";   }
  if (value == color::green) { return "green"; }
  if (value == color::blue)  { return "blue";  }
  return {};
}

void bench_color() {
  std::map<color, const char*> map{
    {color::red, "red"},
    {color::green, "green"},
    {color::blue, "blue"},
  };

  std::unordered_map<color, const char*> unordered_map{
    {color::red, "red"},
    {color::green, "green"},
    {color::blue, "blue"},
  };

  constexpr frozen::unordered_map<color, const char*, 3> frozen_unordered_map{
    {color::red, "red"},
    {color::green, "green"},
    {color::blue, "blue"},
  };

  static constexpr auto mph_color = std::array{
    std::pair{color::red, "red"},
    std::pair{color::green, "green"},
    std::pair{color::blue, "blue"},
  };

  static constexpr auto size = 1'000'000;

  std::mt19937_64 gen{42};
  //std::uniform_int_distribution<int> dis{0, 2};

  ankerl::nanobench::Bench b{};
  //b.title("enum_to_string").warmup(100).performanceCounters(false);

  std::vector probabilities1 = {0.8, 0.1, 0.1};
  std::vector probabilities2 = {0.1, 0.8, 0.1};
  std::vector probabilities3 = {0.1, 0.1, 0.8};

  std::vector<color> lookups1(size);
  std::discrete_distribution<> dist1(probabilities1.begin(), probabilities1.end());
  for (auto i = 0u; i < size; ++i) lookups1[i] = color(dist1(gen));

  std::vector<color> lookups2(size);
  std::discrete_distribution<> dist2(probabilities2.begin(), probabilities2.end());
  for (auto i = 0u; i < size; ++i) lookups2[i] = color(dist2(gen));

  std::vector<color> lookups3(size);
  std::discrete_distribution<> dist3(probabilities3.begin(), probabilities3.end());
  for (auto i = 0u; i < size; ++i) lookups3[i] = color(dist3(gen));


  b.run("if/else[red]", [&] {
    for (const auto& lookup : lookups3) {
      ankerl::nanobench::doNotOptimizeAway(enum_to_string(lookup));
    }
  });

  //b.run("if/else[green]", [&] {
    //for (const auto& lookup : lookups2) {
      //ankerl::nanobench::doNotOptimizeAway(enum_to_string(lookup));
    //}
  //});

  //b.run("if/else[blue]", [&] {
    //for (const auto& lookup : lookups3) {
      //ankerl::nanobench::doNotOptimizeAway(enum_to_string(lookup));
    //}
  //});

  //b.run("std::map", [&] {
    //for (const auto& lookup : lookups) {
      //ankerl::nanobench::doNotOptimizeAway(map.find(lookup)->second);
    //}
  //});

  //b.run("std::unordered_map", [&] {
    //for (const auto& lookup : lookups) {
      //ankerl::nanobench::doNotOptimizeAway(unordered_map.find(lookup)->second);
    //}
  //});

  //b.run("frozen", [&] {
    //for (const auto& lookup : lookups) {
      //ankerl::nanobench::doNotOptimizeAway(frozen_unordered_map.find(lookup)->second);
    //}
  //});

  //b.run("mph", [&] {
    //for (const auto& lookup : lookups) {
      //ankerl::nanobench::doNotOptimizeAway(mph::lookup<mph_color>(lookup));
    //}
  //});
}

//void bench_status() {
  //enum class http_status_code : std::uint32_t {
    //unknown = 0,
    //ok = 200,
    //created = 201,
    //accepted = 202,
    //no_content = 204,
    //bad_request = 400,
    //unauthorized = 401,
    //forbidden = 403,
    //not_found = 404,
    //method_not_allowed = 405,
    //conflict = 409,
    //internal_server_error = 500,
    //not_implemented = 501,
    //bad_gateway = 502,
    //service_unavailable = 503,
  //};

  //std::map<http_status_code, const char*> map{
    //{http_status_code::unknown, "unknown"},
    //{http_status_code::ok, "ok"},
    //{http_status_code::created, "created"},
    //{http_status_code::accepted, "accepted"},
    //{http_status_code::no_content, "no_content"},
    //{http_status_code::bad_request, "bad_request"},
    //{http_status_code::unauthorized, "unauthorized"},
    //{http_status_code::forbidden, "forbidden"},
    //{http_status_code::not_found, "not_found"},
    //{http_status_code::method_not_allowed, "method_not_allowed"},
    //{http_status_code::conflict, "conflict"},
    //{http_status_code::internal_server_error, "internal_server_error"},
    //{http_status_code::not_implemented, "not_implemented"},
    //{http_status_code::bad_gateway, "bad_gateway"},
    //{http_status_code::service_unavailable, "service_unavailable"},
  //};

  //std::unordered_map<http_status_code, const char*> unordered_map{
    //{http_status_code::unknown, "unknown"},
    //{http_status_code::ok, "ok"},
    //{http_status_code::created, "created"},
    //{http_status_code::accepted, "accepted"},
    //{http_status_code::no_content, "no_content"},
    //{http_status_code::bad_request, "bad_request"},
    //{http_status_code::unauthorized, "unauthorized"},
    //{http_status_code::forbidden, "forbidden"},
    //{http_status_code::not_found, "not_found"},
    //{http_status_code::method_not_allowed, "method_not_allowed"},
    //{http_status_code::conflict, "conflict"},
    //{http_status_code::internal_server_error, "internal_server_error"},
    //{http_status_code::not_implemented, "not_implemented"},
    //{http_status_code::bad_gateway, "bad_gateway"},
    //{http_status_code::service_unavailable, "service_unavailable"},
  //};

  //constexpr frozen::unordered_map<http_status_code, const char*, 3> frozen_unordered_map{
    //{http_status_code::unknown, "unknown"},
    //{http_status_code::ok, "ok"},
    //{http_status_code::created, "created"},
    //{http_status_code::accepted, "accepted"},
    //{http_status_code::no_content, "no_content"},
    //{http_status_code::bad_request, "bad_request"},
    //{http_status_code::unauthorized, "unauthorized"},
    //{http_status_code::forbidden, "forbidden"},
    //{http_status_code::not_found, "not_found"},
    //{http_status_code::method_not_allowed, "method_not_allowed"},
    //{http_status_code::conflict, "conflict"},
    //{http_status_code::internal_server_error, "internal_server_error"},
    //{http_status_code::not_implemented, "not_implemented"},
    //{http_status_code::bad_gateway, "bad_gateway"},
    //{http_status_code::service_unavailable, "service_unavailable"},
  //};

  //static constexpr auto mph_color = std::array{
    //std::pair{http_status_code::unknown, "unknown"},
    //std::pair{http_status_code::ok, "ok"},
    //std::pair{http_status_code::created, "created"},
    //std::pair{http_status_code::accepted, "accepted"},
    //std::pair{http_status_code::no_content, "no_content"},
    //std::pair{http_status_code::bad_request, "bad_request"},
    //std::pair{http_status_code::unauthorized, "unauthorized"},
    //std::pair{http_status_code::forbidden, "forbidden"},
    //std::pair{http_status_code::not_found, "not_found"},
    //std::pair{http_status_code::method_not_allowed, "method_not_allowed"},
    //std::pair{http_status_code::conflict, "conflict"},
    //std::pair{http_status_code::internal_server_error, "internal_server_error"},
    //std::pair{http_status_code::not_implemented, "not_implemented"},
    //std::pair{http_status_code::bad_gateway, "bad_gateway"},
    //std::pair{http_status_code::service_unavailable, "service_unavailable"},
  //};

  //static constexpr auto size = 1'000'000;

  //std::vector<http_status_code> lookups(size);
  //std::mt19937_64 gen{42};
  //std::uniform_int_distribution<int> dis{0, 2};
  //for (auto i = 0u; i < size; ++i) lookups[i] = http_status_code(dis(gen));

  //ankerl::nanobench::Bench b{};

  //b.title("enum_to_string").warmup(100).performanceCounters(true);

  //b.run("std::map", [&] {
    //for (const auto& lookup : lookups) {
      //ankerl::nanobench::doNotOptimizeAway(enum_to_string(lookup));
    //}
  //});

  ////b.run("std::map", [&] {
    ////for (const auto& lookup : lookups) {
      ////ankerl::nanobench::doNotOptimizeAway(map.find(lookup)->second);
    ////}
  ////});

  //////b.run("std::unordered_map", [&] {
    //////for (const auto& lookup : lookups) {
      //////ankerl::nanobench::doNotOptimizeAway(unordered_map.find(lookup)->second);
    //////}
  //////});

  ////b.run("frozen", [&] {
    ////for (const auto& lookup : lookups) {
      ////ankerl::nanobench::doNotOptimizeAway(frozen_unordered_map.find(lookup)->second);
    ////}
  ////});

  ////b.run("mph", [&] {
    ////for (const auto& lookup : lookups) {
      ////ankerl::nanobench::doNotOptimizeAway(mph::lookup<mph_color>(lookup));
    ////}
  ////});
//}

int main() {
  bench_color();
  //bench_status();

  // boost unordered_flat_map
  // switch-case
  // simd
  // frozen
}

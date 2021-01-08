// unique_resource
// Copyright (c) 2015 okdshin
// Portions Copyright (c) 2020 D3 Engineering, LLC.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <utility>
#include <string>
#include <sstream>
#include <memory>
#include <cassert>

#include "unique_resource.hpp"

void test_semantics() {
  std::ostringstream out{};
  {
    auto res = std_experimental::make_unique_resource(
        1, [&out](int i) { out << "cleaned " << i; });
  }
  assert("cleaned 1" == out.str());
}
void test_semantics_reset() {
  std::ostringstream out{};
  {
    auto cleanup = std_experimental::make_unique_resource(
        1, [&out](int i) { out << "cleaned " << i; });
    cleanup.reset(2);
  }
  assert("cleaned 1cleaned 2" == out.str());
}

#if __cplusplus >= 201402
void test_semantics_reset_move() {
  std::ostringstream out{};
  {
    auto cleanup = std_experimental::make_unique_resource(
        std::make_unique<int>(42),
        [&out](auto const&) { out << "cleaned "; });
    cleanup.reset(nullptr);
  }
  assert("cleaned cleaned " == out.str());
}
#endif

void test_semantics_release() {
  std::ostringstream out{};
  {
    auto cleanup = std_experimental::make_unique_resource(
        5, [&out](int i) { out << "cleaned " << i; });
    assert(5 == cleanup.release());
  }
  assert("" == out.str());
}
void test_with_pointer() {
  std::ostringstream out{};
  {
    auto cleanup = std_experimental::make_unique_resource(
        static_cast<char const* const>("hello"),
        [&out](const char* s) { out << "cleaned " << s; });
    assert('h' == *cleanup);
  }
  assert("cleaned hello" == out.str());
}
void test_address_of() {
  std::ostringstream out{};
  {
    int j = 4;
    auto cleanup = std_experimental::make_unique_resource(
        std::move(j), [&out](int i) { out << "cleaned " << i; });
    assert(j == *&cleanup);
  }
  assert("cleaned 4" == out.str());
}
void test_with_failure_value() {
  std::ostringstream out{};
  {
    auto cleanup = std_experimental::make_unique_resource_checked(
        -1, -1, [&out](int i) { out << "cleaned " << i; });
    assert(-1 == cleanup.release());
  }
  assert("" == out.str());
}
void test_move_enable() {
  std::ostringstream out{};
  {
    auto cleanup = std_experimental::make_unique_resource(
        -1, [&out](int i) { out << "cleaned " << i; });
    auto cleanup2 = std::move(cleanup);
    cleanup2.release();
    cleanup2.reset(42);
  }
  assert("cleaned 42" == out.str());
}

#if __cplusplus >= 201402
auto pass_unique_resource(std::ostream& out) {
  return std_experimental::make_unique_resource(
      std::make_unique<int>(42), [&out](auto& i) { out << "cleaned " << *i; });
}
void test_unique_resource_can_be_moved() {
  std::ostringstream out{};
  {
    auto cleanup = pass_unique_resource(out);
    assert(42 == *cleanup.get());
  }
  assert("cleaned 42" == out.str());
}
#endif

void thrower(int) noexcept(false) { throw 42; }
void test_noexcept_deleter() {
  //auto cleanup = std_experimental::make_unique_resource(42, &thrower);
  //will terminate if run....
}
int main(int /* argc */, char *argv[]) {
  std::cout << argv[0] << " running tests" << std::endl;
  test_semantics();
  test_semantics_reset();
#if __cplusplus >= 201402
  test_semantics_reset_move();
#endif
  test_semantics_release();
  test_with_pointer();
  test_address_of();
  test_with_failure_value();
  test_move_enable();
#if __cplusplus >= 201402
  test_unique_resource_can_be_moved();
#endif
  test_noexcept_deleter();
  std::cout << argv[0] << " tests have passed" << std::endl;
  return 0;
}

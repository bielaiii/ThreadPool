#include "pools.hpp"
#include "test.hpp"
#include <print>
#include <type_traits>
using namespace std;

struct foo {
    foo(foo &&) = delete;
};

struct foo_no {
    foo_no(foo_no &&) noexcept {};
};
int main() {
    println("{}", is_nothrow_move_constructible<foo_no>::value);
    println("{}", is_nothrow_move_constructible<foo>::value);
    println("{}", Compact::all_are_safely_move_copy<int, foo, foo_no>::value);
    CompactTest::run_all_tests();
    std::cout << "All static tests passed successfully!" << std::endl;
    return 0;
}

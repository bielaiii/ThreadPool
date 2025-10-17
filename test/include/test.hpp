// include/test_pools.hpp
#ifndef TEST_POOLS_HPP
#define TEST_POOLS_HPP

#include "pools.hpp"
#include <memory>
#include <type_traits>

namespace CompactTest {

// Test class that is copyable but not nothrow moveable
class CopyableOnly {
public:
    CopyableOnly() = default;
    CopyableOnly(const CopyableOnly&) = default;
    CopyableOnly& operator=(const CopyableOnly&) = default;
    CopyableOnly(CopyableOnly&&) noexcept(false) {};
    CopyableOnly& operator=(CopyableOnly&&) noexcept(false) = default;
};

// Test class that is nothrow moveable but not copyable
class MoveOnlyNothrow {
public:
    MoveOnlyNothrow() = default;
    MoveOnlyNothrow(const MoveOnlyNothrow&) = delete;
    MoveOnlyNothrow& operator=(const MoveOnlyNothrow&) = delete;
    MoveOnlyNothrow(MoveOnlyNothrow&&) noexcept = default;
    MoveOnlyNothrow& operator=(MoveOnlyNothrow&&) noexcept = default;
};

// Test class that is neither copyable nor moveable
class NonCopyableNonMoveable {
public:
    NonCopyableNonMoveable() = default;
    NonCopyableNonMoveable(const NonCopyableNonMoveable&) = delete;
    NonCopyableNonMoveable& operator=(const NonCopyableNonMoveable&) = delete;
    NonCopyableNonMoveable(NonCopyableNonMoveable&&) = delete;
    NonCopyableNonMoveable& operator=(NonCopyableNonMoveable&&) = delete;
};
#if 1
void run_all_tests() {
    // Test 1: Basic types (should be true)
    static_assert(Compact::all_are_safely_move_copy<int>::value, "Test failed for int");
    static_assert(Compact::all_are_safely_move_copy<int, double>::value, "Test failed for int, double");
    
    // Test 2: std::unique_ptr (move-only type, should be true)
    static_assert(Compact::all_are_safely_move_copy<std::unique_ptr<int>>::value, 
                 "Test failed for unique_ptr");
    
    // Test 3: Mixed types (should be true)
    static_assert(Compact::all_are_safely_move_copy<int, std::unique_ptr<int>>::value, 
                 "Test failed for int, unique_ptr");
    
    // Test 4: Custom types
    static_assert(Compact::all_are_safely_move_copy<CopyableOnly>::value, 
                 "Test failed for CopyableOnly");
    static_assert(Compact::all_are_safely_move_copy<MoveOnlyNothrow>::value, 
                 "Test failed for MoveOnlyNothrow");
    static_assert(!Compact::all_are_safely_move_copy<NonCopyableNonMoveable>::value, 
                 "Test failed for NonCopyableNonMoveable");
    
    // Test 5: Multiple mixed custom types
    static_assert(Compact::all_are_safely_move_copy<CopyableOnly, MoveOnlyNothrow>::value, 
                 "Test failed for mixed custom types");
}
#endif
} // namespace CompactTest

#endif // TEST_POOLS_HPP
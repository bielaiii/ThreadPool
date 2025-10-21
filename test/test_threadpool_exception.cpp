#include "pools.hpp"
#include <cassert>
#include <exception>
#include <iostream>
#include <stdexcept>

int main() {
    Compact::ThreadPools pool(2);

    auto f = pool.enqueue([]() -> int {
        throw std::runtime_error("task error");
        return 0;
    });

    // get() 应当抛出 task 内的异常
    bool caught = false;
    try {
        f.get();
    } catch (const std::runtime_error &e) {
        caught = true;
        std::string msg = e.what();
        assert(msg == "task error");
    }
    assert(caught);

    pool.wait_all();
    std::cout << "[exception] OK\n";
    return 0;
}
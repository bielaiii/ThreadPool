#include "pools.hpp"
#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    Compact::ThreadPools pool(8);

    std::atomic<int> active{0};
    std::atomic<int> max_active{0};
    const int tasks = 1000;

    std::vector<std::future<void>> futs;
    futs.reserve(tasks);

    for (int i = 0; i < tasks; ++i) {
        futs.push_back(pool.enqueue([&]() {
            active.fetch_add(1, std::memory_order_acq_rel);
            int cur = active.load(std::memory_order_acquire);
            int prev = max_active.load(std::memory_order_acquire);
            while (cur > prev && !max_active.compare_exchange_weak(prev, cur)) {
                // try again
            }
            std::this_thread::sleep_for(std::chrono::microseconds(50));
            active.fetch_sub(1, std::memory_order_acq_rel);
        }));
    }

    // 等待所有任务完成
    pool.wait_all();

    // 所有 futures 可安全 get (void)
    for (auto &f : futs) f.get();

    // max_active 至少为 1，且不应超过线程数（8）
    assert(max_active.load() >= 1);
    assert(max_active.load() <= 8);

    std::cout << "[concurrency] OK (max_active=" << max_active.load() << ")\n";
    return 0;
}
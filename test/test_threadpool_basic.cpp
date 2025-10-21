#include "pools.hpp"
#include "test.hpp"
#include <cassert>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace std::chrono_literals;

int main_basic() {
    Compact::ThreadPools pool(4);

    std::atomic<int> counter{0};
    std::vector<std::future<int>> futs;

    // 提交 100 个任务，每个任务递增 counter 并返回索引
    for (int i = 0; i < 100; ++i) {
        futs.push_back(pool.enqueue([i, &counter]() -> int {
            // 模拟工作
            std::this_thread::sleep_for(1ms);
            counter.fetch_add(1, std::memory_order_relaxed);
            std::cout << "Tasks " << counter << " complete\n"; 
            return i;
        }));
    }

    // pool 内部 working_count 在任务入队时就被 ++，因此只能用 wait_all 判断完成
    pool.wait_all();

    // 所有 future 应该可以成功取到正确结果
    for (int i = 0; i < 100; ++i) {
        int v = futs[i].get();
        assert(v >= 0 && v < 100);
    }

    assert(counter.load() == 100);

    // 检查 get_statue 返回合理值
    auto s = pool.get_statue();
    assert(s.pending_tasks == 0);
    assert(s.working_threads == 0);

    std::cout << "[basic] OK\n";
    return 0;
}

int main_timeout() {
    Compact::ThreadPools pool(2);

    // 提交一个较慢任务
    auto f = pool.enqueue([]() {
        std::this_thread::sleep_for(100ms);
        return 42;
    });

    // 很短的超时，应当返回 timeout
    auto st = pool.enqueue_timeout(10ms, []() {
        std::this_thread::sleep_for(100ms);
        return 1;
    });
    assert(st == std::future_status::timeout);

    // 如果等待足够长，应该变为 ready
    auto st2 = pool.enqueue_timeout(200ms, []() {
        std::this_thread::sleep_for(50ms);
        return 2;
    });
    assert(st2 == std::future_status::ready);

    // 清理
    pool.wait_all();
    (void)f.get();

    std::cout << "[timeout] OK\n";
    return 0;
}

int main() {
    // 先运行静态 trait 测试（test.hpp 中的 static_asserts 会在编译期检查）
    CompactTest::run_all_tests();

    main_basic();
    main_timeout();

    std::cout << "All runtime basic tests passed\n";
    return 0;
}
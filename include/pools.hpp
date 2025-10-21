#ifndef POOLS_HEADER
#define POOLS_HEADER
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
namespace Compact {

template<typename... Args>
struct all_are_safely_move_copy
    : std::bool_constant<

          std::conjunction_v<std::disjunction<
              std::is_copy_constructible<Args>,
              std::conjunction<std::is_move_constructible<Args>,
                               std::is_nothrow_move_constructible<Args>>>...>>

{};

class ThreadPools {
    private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::condition_variable cv;
    std::mutex mutex_;
    bool should_terminate;
    std::atomic<bool> cleaned{false};
    std::atomic<size_t> working_count{0};

    public:
    struct PoolStatus {
        size_t total_threads;
        size_t working_threads;
        size_t pending_tasks;
        bool is_shutting_down;
    };

    ThreadPools() noexcept : should_terminate(false) {};
    ThreadPools(size_t N) noexcept : should_terminate(false) {

        for (int i = 0; i < N; i++) {

            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock unique_lock_{this->mutex_};
                        cv.wait(unique_lock_, [this] {
                            return this->should_terminate ||
                                   !this->tasks.empty();
                        });
                        if (this->should_terminate && this->tasks.empty()) {
                            return;
                        }
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
        };
    }

    PoolStatus get_statue() {
        std::lock_guard lock(mutex_);
        return PoolStatus{.total_threads = workers.size(),
                          .working_threads =
                              working_count.load(std::memory_order::acquire),
                          .pending_tasks = tasks.size(),
                          .is_shutting_down = should_terminate};
    }

    template<typename F, typename... Args>
    auto enqueue(F call_, Args &&...args) {

        using ret_t = std::invoke_result_t<F, Args...>;
        auto bind_helper = std::bind(call_, std::forward<Args>(args)...);
        auto tk = std::packaged_task<ret_t()>(std::move(bind_helper));
        this->working_count.fetch_add(1);
        auto share_ptr_ = std::shared_ptr<std::packaged_task<ret_t()>>(
            new std::packaged_task<ret_t()>(std::move(tk)),
            [=, this](std::packaged_task<ret_t()> *p) {
                this->working_count.fetch_sub(1, std::memory_order::release);
                delete p;
                cv.notify_all();
            });

        std::future<ret_t> ret_ = share_ptr_.get()->get_future();
        {
            std::lock_guard lock(mutex_);
            tasks.push([share_ptr_]() { (*share_ptr_)(); });
        }
        using namespace std::chrono_literals;
        cv.notify_one();
        return ret_;
    }

    template<typename F, typename... Args>
    auto enqueue_timeout(std::chrono::milliseconds ms, F call_,
                         Args &&...args) {

        auto ret_ = enqueue(call_, std::forward<Args>(args)...);
        return ret_.wait_for(ms);
    }

    int stop() noexcept {
        bool expected = false;
        if (!cleaned.compare_exchange_strong(expected, true)) {
            return 1;
        }
        {
            std::lock_guard lock(mutex_);

            should_terminate = true;
        }

        cv.notify_all();
        for (auto &thread : workers) {
            thread.join();
        }
        return 0;
    }

    void wait_all() {
        std::unique_lock lock_(mutex_);
        cv.wait(lock_, [this]() {
            return tasks.empty() &&
                   (working_count.load(std::memory_order::acquire) == 0);
        });
    }

    ~ThreadPools() noexcept {
        stop();
    }
};

} // namespace Compact
#endif

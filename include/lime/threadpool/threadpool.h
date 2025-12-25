#ifndef LIME_HTTP_THREADPOOL_H
#define LIME_HTTP_THREADPOOL_H

#include <condition_variable>
#include <atomic>
#include <cstddef>
#include <functional>
#include <mutex>
#include <stop_token>
#include <thread>
#include <queue>

namespace lime {
  class DynamicThreadPool {
    public:
    explicit DynamicThreadPool(size_t max_workers = std::thread::hardware_concurrency());
    ~DynamicThreadPool();
    void enqueue(std::function<void()>);
    void shutdown();

    private:
    void worker(const size_t id, std::stop_token);

    std::vector<std::jthread>         m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::condition_variable           m_task_available;
    std::mutex                        m_tasks_mutex;
    std::atomic<bool>                 m_stop = false;
  };
} // lime

#endif // LIME_HTTP_THREADPOOL_H

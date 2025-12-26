#include <lime/lime.h>
#include <mutex>
#include <format>

namespace lime {
  DynamicThreadPool::DynamicThreadPool(size_t max_workers) {
    debug(std::format("setting up {} workers", max_workers));
    for (size_t i = 0; i < max_workers; i++) {
      m_workers.emplace_back([i, this](std::stop_token stoken) {
        this->worker(i, stoken);
      });
    }
  }

  DynamicThreadPool::~DynamicThreadPool() {
    if (!m_stop)
      shutdown();
  }

  void DynamicThreadPool::enqueue(std::function<void()> func) {
    {
      std::lock_guard lock { m_tasks_mutex };
      m_tasks.emplace(
        std::forward<std::function<void()>>(func)
      );
    }
    m_task_available.notify_one();
  }

  void DynamicThreadPool::shutdown() {
    debug("shutting down workers");
    if (!m_stop.exchange(true)) {
      m_task_available.notify_all();
    }
  }

  void DynamicThreadPool::worker(const size_t id, std::stop_token stoken) {
    while (!stoken.stop_requested()) {
      std::function<void()> task {};

      {
        std::unique_lock lock { m_tasks_mutex };

        debug(std::format("thread: {}: waiting for task", id));
        m_task_available.wait(lock, [this, &stoken]() {
          return m_stop || !m_tasks.empty() || stoken.stop_requested();
        });

        if (m_stop && m_tasks.empty())
          return;

        debug(std::format("thread: {}: dequeuing task from queue", id));
        task = std::move(m_tasks.front());
        m_tasks.pop();
      }

      debug(std::format("thread: {}: running task", id));
      task();
    }
  }
} // lime

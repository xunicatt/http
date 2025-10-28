#include <print>
#include <threadpool.h>
#include <mutex>

DynamicThreadPool::DynamicThreadPool(size_t max_workers) {
  for (size_t i = 0; i < max_workers; i++) {
    workers.emplace_back([this](std::stop_token stoken) {
      this->worker(stoken);
    });
  }
}

DynamicThreadPool::~DynamicThreadPool() {
  shutdown();
}

void DynamicThreadPool::enqueue(std::function<void()> func) {
  {
    std::lock_guard lock(tasks_mutex);
    tasks.emplace(
      std::forward<std::function<void()>>(func)
    );
  }
  task_available.notify_one();
}

void DynamicThreadPool::shutdown() {
  if (!stop.exchange(true)) {
    task_available.notify_all();
  }
}

void DynamicThreadPool::worker(std::stop_token stoken) {
  while (!stoken.stop_requested()) {
    std::function<void()> task;

    {
      std::unique_lock lock(tasks_mutex);
      task_available.wait(lock, [this, &stoken]() {
        return stop || !tasks.empty() || stoken.stop_requested();
      });

      if (stop && tasks.empty())
        return;

      task = std::move(tasks.front());
      tasks.pop();
    }

    task();
  }
}


#include <threadpool.h>
#include <mutex>
#include <logger.h>
#include <format>

DynamicThreadPool::DynamicThreadPool(size_t max_workers) {
  http::debug(std::format("setting up {} workers", max_workers));
  for (size_t i = 0; i < max_workers; i++) {
    workers.emplace_back([i, this](std::stop_token stoken) {
      this->worker(i, stoken);
    });
  }
}

DynamicThreadPool::~DynamicThreadPool() {
  if (!stop)
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
  http::debug("shutting down workers");
  if (!stop.exchange(true)) {
    task_available.notify_all();
  }
}

void DynamicThreadPool::worker(const size_t id, std::stop_token stoken) {
  while (!stoken.stop_requested()) {
    std::function<void()> task;

    {
      std::unique_lock lock(tasks_mutex);

      http::debug(std::format("thread: {}: waiting for task", id));
      task_available.wait(lock, [this, &stoken]() {
        return stop || !tasks.empty() || stoken.stop_requested();
      });

      if (stop && tasks.empty())
        return;

      http::debug(std::format("thread: {}: dequeuing task from queue", id));
      task = std::move(tasks.front());
      tasks.pop();
    }

    http::debug(std::format("thread: {}: running task", id));
    task();
  }
}


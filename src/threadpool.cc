#include <threadpool.h>
#include <mutex>

template <size_t N>
StaticThreadPool<N>::StaticThreadPool() {
  for (auto& worker: workers) {
    worker = std::jthread([this](std::stop_token stoken) {
      this->worker(stoken);
    });
  }
}

template <size_t N>
StaticThreadPool<N>::~StaticThreadPool() {
  shutdown();
}

template <size_t N>
void StaticThreadPool<N>::enqueue(std::function<void()> func) {
  {
    std::lock_guard lock(tasks_mutex);
    tasks.emplace(
      std::forward<std::function<void()>>(func)
    );
  }
  task_available.notify_one();
}

template <size_t N>
void StaticThreadPool<N>::shutdown() {
  if (!stop.exchange(true)) {
    task_available.notify_all();
  }
}

template <size_t N>
void StaticThreadPool<N>::worker(std::stop_token stoken) {
  std::function<void()> task;

  {
    std::lock_guard lock(tasks_mutex);
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

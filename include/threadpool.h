#ifndef __HTTP_THREADPOOL__
#define __HTTP_THREADPOOL__

#include <condition_variable>
#include <atomic>
#include <cstddef>
#include <functional>
#include <mutex>
#include <stop_token>
#include <thread>
#include <queue>

#ifndef THREAD_POOL_WORKERS
  #define THREAD_POOL_WORKERS 10
#endif

template <size_t N = THREAD_POOL_WORKERS>
class StaticThreadPool {
public:
  StaticThreadPool();
  ~StaticThreadPool();
  void enqueue(std::function<void()>);
  void shutdown();

private:
  void worker(std::stop_token);

  std::array<std::jthread, N> workers;
  std::queue<std::function<void()>> tasks;
  std::condition_variable task_available;
  std::mutex tasks_mutex;
  std::atomic<bool> stop = false;
};

#endif // __HTTP_THREADPOOL__

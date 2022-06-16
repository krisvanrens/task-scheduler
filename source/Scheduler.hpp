#pragma once

#include <cstdio> // XXX

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

template<typename Task, unsigned int MaxQueueLength>
requires(MaxQueueLength < 8192) class Scheduler final {
  std::queue<Task> queue_;
  const unsigned int num_executors_;
  std::vector<std::jthread> executors_;

  void executor() {
    // TODO
    printf("Executing..\n");
  }

  void create_executors() {
    for (unsigned int i = 0; i < num_executors_; i++) {
      executors_.emplace_back(&Scheduler<Task, MaxQueueLength>::executor, this);
    }
  }

public:
  Scheduler(unsigned int num_executors)
    : num_executors_{num_executors} {
    if (num_executors_ > std::jthread::hardware_concurrency()) {
      throw std::overflow_error("Too many executors requested for hardware support");
    }

    create_executors();
  }

  Scheduler(Scheduler&&) noexcept            = default;
  Scheduler& operator=(Scheduler&&) noexcept = default;

  [[nodiscard]] bool schedule(Task&& task) noexcept {
    if (queue_.size() < MaxQueueLength) {
      queue_.push(std::move(task));
      return true;
    }

    return false;
  }

  // XXX
  void run_all() {
    while (!queue_.empty()) {
      queue_.front()();
      queue_.pop();
    }
  }
};

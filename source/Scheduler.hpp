#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#include "Multiqueue.hpp"

template<typename Task, unsigned int MaxQueueLength>
requires(MaxQueueLength < 8192) class Scheduler final {
  const unsigned int               num_executors_;
  Multiqueue<Task, MaxQueueLength> queue_;
  std::vector<std::jthread>        executors_;

  void executor(std::stop_token stop_token, unsigned int id) {
    while (!stop_token.stop_requested()) {
      // TODO
      queue_.pop(id).value_or([] {})();
    }
  }

  void create_executors() {
    for (unsigned int i = 0; i < num_executors_; i++) {
      executors_.emplace_back(std::bind_front(&Scheduler<Task, MaxQueueLength>::executor, this), i);
    }
  }

public:
  Scheduler(unsigned int num_executors)
    : num_executors_{num_executors}
    , queue_{num_executors} {
    if (num_executors_ == 0) {
      throw std::underflow_error("At least one executor must be requested");
    }

    if (num_executors_ > std::jthread::hardware_concurrency()) {
      throw std::overflow_error("Too many executors requested for hardware support");
    }

    create_executors();
  }

  Scheduler(Scheduler&&) noexcept            = default;
  Scheduler& operator=(Scheduler&&) noexcept = default;

  [[nodiscard]] bool schedule(Task&& task) {
    return queue_.push(std::move(task));
  }
};

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#include "Multiqueue.hpp"
#include "Task.hpp"

template<unsigned int MaxQueueLength>
requires(MaxQueueLength < 8192) class SimpleScheduler final {
  std::size_t                              num_executors_;
  Multiqueue<Task<void()>, MaxQueueLength> queue_;
  std::vector<std::jthread>                executors_;

  void executor(std::stop_token stop_token, [[maybe_unused]] unsigned int id) {
    while (!stop_token.stop_requested()) {
      // TODO
    }
  }

  void create_executors() {
    for (unsigned int i = 0; i < static_cast<unsigned int>(num_executors_); i++) {
      executors_.emplace_back(std::bind_front(&SimpleScheduler<MaxQueueLength>::executor, this), i);
    }
  }

public:
  SimpleScheduler(std::size_t num_executors)
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

  SimpleScheduler(SimpleScheduler&&) noexcept            = default;
  SimpleScheduler& operator=(SimpleScheduler&&) noexcept = default;

  [[nodiscard]] constexpr std::size_t num_executors() const {
    return executors_.size();
  }

  [[nodiscard]] bool schedule(Task<void()>&& task) {
    return queue_.push(std::move(task));
  }
};

#pragma once

#include <queue>

template<typename Task, unsigned int MaxQueueLength>
requires(MaxQueueLength < 8192) class Scheduler final {
  std::queue<Task> queue_;

public:
  Scheduler() = default;

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

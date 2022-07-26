#pragma once

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <latch>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "completion_token.hpp"
#include "multiqueue.hpp"
#include "task.hpp"

namespace ts {

inline namespace v1 {

/**
 * Simple scheduler. A thread pool with an associated work queue per executor. This scheduler is able to handle tasks
 *  with signature 'void()`, so it can be used to schedule tasks wrapped in a lambda expression. A multiqueue is used
 *  to implement work stealing for executors: when their respective work queue is empty, work is taken from another
 *  executors' queue. At schedule time, a completion token is returned for the callee to wait on task completion.
 */
template<unsigned int MaxQueueLength>
requires(MaxQueueLength < 8192) class simple_scheduler final {
  struct simple_job {
    task<void()>                             task_;
    std::shared_ptr<detail::completion_data> completion_;
  };

  std::size_t                            num_executors_;
  multiqueue<simple_job, MaxQueueLength> queue_;
  std::vector<std::jthread>              executors_;
  std::latch                             executors_started_;
  std::mutex                             work_mutex_;
  std::condition_variable                work_cv_;

  void executor(std::stop_token stop_token, unsigned int id) {
    executors_started_.arrive_and_wait();

    while (!stop_token.stop_requested()) {
      if (auto&& job = queue_.pop(id); job) {
        (*job).task_();
        (*job).completion_->trigger_completion();
      } else {
        std::unique_lock lock{work_mutex_};
        work_cv_.wait(lock, [&] { return !queue_.empty() || stop_token.stop_requested(); });
      }
    }
  }

  void create_executors() {
    for (unsigned int i = 0; i < static_cast<unsigned int>(num_executors_); i++) {
      executors_.emplace_back(std::bind_front(&simple_scheduler<MaxQueueLength>::executor, this), i);
    }

    executors_started_.arrive_and_wait();
  }

public:
  explicit simple_scheduler(std::size_t num_executors)
    : num_executors_{num_executors}
    , queue_{num_executors}
    , executors_started_{static_cast<std::ptrdiff_t>(num_executors + 1)} { // +1 for the main thread.
    if (num_executors_ == 0) {
      throw std::underflow_error("At least one executor must be requested");
    }

    if (num_executors_ > std::jthread::hardware_concurrency()) {
      throw std::overflow_error("Too many executors requested for hardware support");
    }

    create_executors();
  }

  ~simple_scheduler() {
    std::unique_lock lock{work_mutex_};
    std::ranges::for_each(executors_, [](auto& executor) { executor.request_stop(); });
    work_cv_.notify_all();
  }

  simple_scheduler(const simple_scheduler&) noexcept            = delete;
  simple_scheduler& operator=(const simple_scheduler&) noexcept = delete;

  [[nodiscard]] constexpr std::size_t num_executors() const {
    return executors_.size();
  }

  [[nodiscard]] std::optional<completion_token> schedule(task<void()>&& task) {
    auto completion = std::make_shared<detail::completion_data>();
    auto job        = simple_job{std::move(task), completion};

    if (queue_.push(std::move(job))) {
      work_cv_.notify_one();
      return completion_token{completion};
    } else {
      task = std::move(job.task_); // Hand back the task in case scheduling failed.
    }

    return {};
  }

  void flush() {
    queue_.flush();
  }
};

} // namespace v1

} // namespace ts

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

#include "CompletionToken.hpp"
#include "Multiqueue.hpp"
#include "Task.hpp"

namespace ts {

inline namespace v1 {

/**
 * Simple scheduler. A thread pool with an associated work queue per executor. This scheduler is able to handle tasks
 *  with signature 'void()`, so it can be used to schedule tasks wrapped in a lambda expression. A Multiqueue is used
 *  to implement work stealing for executors: when their respective work queue is empty, work is taken from another
 *  executors' queue. At schedule time, a completion token is returned for the callee to wait on task completion.
 */
template<unsigned int MaxQueueLength>
requires(MaxQueueLength < 8192) class SimpleScheduler final {
  struct Job {
    Task<void()>                            task_;
    std::shared_ptr<detail::CompletionData> completion_;
  };

  std::size_t                     num_executors_;
  Multiqueue<Job, MaxQueueLength> queue_;
  std::vector<std::jthread>       executors_;
  std::latch                      executors_started_;
  std::mutex                      work_mutex_;
  std::condition_variable         work_cv_;

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
      executors_.emplace_back(std::bind_front(&SimpleScheduler<MaxQueueLength>::executor, this), i);
    }

    executors_started_.arrive_and_wait();
  }

public:
  explicit SimpleScheduler(std::size_t num_executors)
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

  ~SimpleScheduler() {
    std::unique_lock lock{work_mutex_};
    std::ranges::for_each(executors_, [](auto& executor) { executor.request_stop(); });
    work_cv_.notify_all();
  }

  SimpleScheduler(const SimpleScheduler&) noexcept            = delete;
  SimpleScheduler& operator=(const SimpleScheduler&) noexcept = delete;

  [[nodiscard]] constexpr std::size_t num_executors() const {
    return executors_.size();
  }

  [[nodiscard]] std::optional<CompletionToken> schedule(Task<void()>&& task) {
    auto completion = std::make_shared<detail::CompletionData>();
    auto job        = Job{std::move(task), completion};

    if (queue_.push(std::move(job))) {
      work_cv_.notify_one();
      return CompletionToken{completion};
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

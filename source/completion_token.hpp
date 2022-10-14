#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace ts {

inline namespace v1 {

namespace detail {

class completion_data final {
  mutable std::mutex      mutex_;
  std::condition_variable condition_;
  bool                    completed_{false};

public:
  [[nodiscard]] bool is_completed() const {
    std::unique_lock lock{mutex_};
    return completed_;
  }

  void wait_for_completion() {
    std::unique_lock lock{mutex_};
    condition_.wait(lock, [this] { return this->completed_; });
  }

  void trigger_completion() {
    std::unique_lock lock{mutex_};
    completed_ = true;
    condition_.notify_all();
  }
};

} // namespace detail

class completion_token final {
  std::shared_ptr<detail::completion_data> data_;

public:
  explicit completion_token(std::shared_ptr<detail::completion_data> data)
    : data_{data} {
    if (!data_) {
      throw std::logic_error("Completion data must not be null");
    }
  }

  void wait() {
    data_->wait_for_completion();
  }

  [[nodiscard]] operator bool() const {
    return data_->is_completed();
  }
};

} // namespace v1

} // namespace ts

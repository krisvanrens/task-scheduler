#pragma once

#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>

namespace ts {

inline namespace v1 {

namespace detail {

class completion_data final {
  mutable std::mutex                mutex_;
  mutable std::condition_variable   condition_;
  bool                              completed_{false};
  std::optional<std::exception_ptr> exception_;

public:
  [[nodiscard]] bool is_completed() const {
    std::unique_lock lock{mutex_};
    return completed_;
  }

  void wait_for_completion() const {
    std::unique_lock lock{mutex_};
    condition_.wait(lock, [this] { return this->completed_; });
  }

  void trigger_completion() {
    std::unique_lock lock{mutex_};
    completed_ = true;
    condition_.notify_all();
  }

  [[nodiscard]] const std::optional<std::exception_ptr>& exception() const {
    std::unique_lock lock{mutex_};
    return exception_;
  }

  [[nodiscard]] std::optional<std::exception_ptr>& exception() {
    std::unique_lock lock{mutex_};
    return exception_;
  }
};

} // namespace detail

///
/// Completion token, a token used to check and wait for associated entity completion.
///
class completion_token final {
  std::shared_ptr<detail::completion_data> data_;

public:
  ///
  /// Constructor.
  ///
  /// \param data The associated completion data.
  ///
  /// \throws `std::logic_error` if the completion data is empty.
  ///
  explicit completion_token(std::shared_ptr<detail::completion_data> data)
    : data_{data} {
    if (!data_) {
      throw std::logic_error("Completion data must not be null");
    }
  }

  ///
  /// Blocking wait for completion of the associated entity.
  ///
  void wait() const {
    data_->wait_for_completion();
  }

  ///
  /// Check for completion.
  ///
  /// \returns True if associated entity is completed, false if not.
  ///
  [[nodiscard]] operator bool() const {
    return data_->is_completed();
  }

  ///
  /// Check if an exception was thrown during completion of the associated entity.
  ///
  /// \returns An optional exception object. Will be empty if there was no exception, or if the token is not completed.
  ///
  [[nodiscard]] std::optional<std::exception_ptr> exception() const {
    return data_->exception();
  }
};

} // namespace v1

} // namespace ts

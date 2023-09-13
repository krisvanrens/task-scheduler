#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>

static constexpr std::size_t MAX_SIZE_LIMIT{8192};

namespace ts {

inline namespace v1 {

static constexpr std::size_t safe_queue_max_size_limit{MAX_SIZE_LIMIT};

///
/// Thread-safe queue (FIFO).
///
/// \param T       The queue element value type.
/// \param MaxSize The maximum queue size. Must be in range 1..MAX_SIZE_LIMIT.
///
template<typename T, std::size_t MaxSize>
requires((MaxSize > 0) && (MaxSize <= MAX_SIZE_LIMIT)) class safe_queue final {
  std::deque<T>             queue_;
  mutable std::shared_mutex mutex_;

public:
  safe_queue() = default;

  safe_queue(safe_queue& other) noexcept      = delete;
  safe_queue& operator=(safe_queue&) noexcept = delete;

  ///
  /// Get the maximum queue length limit.
  ///
  /// \returns The maximum queue length limit.
  ///
  [[nodiscard]] static constexpr std::size_t max_size_limit() noexcept {
    return MAX_SIZE_LIMIT;
  }

  ///
  /// Get the maximum queue size.
  ///
  /// \returns The maximum queue length.
  ///
  [[nodiscard]] static constexpr std::size_t max_size() noexcept {
    return MaxSize;
  }

  ///
  /// Get the current queue size.
  ///
  /// \returns The current queue length.
  ///
  [[nodiscard]] std::size_t size() const {
    std::shared_lock lock{mutex_};

    return queue_.size();
  }

  ///
  /// Check if the queue is empty.
  ///
  /// \returns `true` if the queue is empty, `false` if otherwise.
  ///
  [[nodiscard]] bool empty() const {
    std::shared_lock lock{mutex_};

    return queue_.empty();
  }

  ///
  /// Push a new element into the back of the queue.
  ///
  /// \param element The element to push on the queue.
  ///
  /// \returns `true` if the element is accepted, `false` if the queue could not accept the element (because maximum
  ///           occupation capacity is reached).
  ///
  template<typename U>
  [[nodiscard]] bool push(U&& element) {
    std::unique_lock lock{mutex_};

    if (queue_.size() >= MaxSize) {
      return false;
    }

    queue_.push_back(std::forward<U>(element));

    return true;
  }

  ///
  /// Pop an element off the front of the queue.
  ///
  /// \returns An optional element. The optional is empty if the queue was empty.
  ///
  [[nodiscard]] std::optional<T> pop() {
    std::unique_lock lock{mutex_};
    std::optional<T> result;

    if (queue_.empty()) {
      return result;
    }

    result = std::move(queue_.front());
    queue_.pop_front();

    return result;
  }

  ///
  /// Flush the queue, removing all elements.
  ///
  void flush() {
    std::unique_lock lock{mutex_};
    queue_.clear();
  }
};

} // namespace v1

} // namespace ts

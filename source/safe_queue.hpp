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

/// Thread-safe queue with direct pop-result support.
template<typename T, std::size_t MaxSize>
requires((MaxSize > 0) && (MaxSize <= MAX_SIZE_LIMIT)) class safe_queue final {
  std::deque<T>             queue_;
  mutable std::shared_mutex mutex_;

public:
  safe_queue() = default;

  safe_queue(safe_queue& other) noexcept      = delete;
  safe_queue& operator=(safe_queue&) noexcept = delete;

  [[nodiscard]] static constexpr std::size_t max_size_limit() noexcept {
    return MAX_SIZE_LIMIT;
  }

  [[nodiscard]] static constexpr std::size_t max_size() noexcept {
    return MaxSize;
  }

  [[nodiscard]] std::size_t size() const {
    std::shared_lock lock{mutex_};

    return queue_.size();
  }

  [[nodiscard]] bool empty() const {
    std::shared_lock lock{mutex_};

    return queue_.empty();
  }

  template<typename U>
  [[nodiscard]] bool push(U&& element) {
    std::unique_lock lock{mutex_};

    if (queue_.size() >= MaxSize) {
      return false;
    }

    queue_.push_back(std::forward<U>(element));

    return true;
  }

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

  void flush() {
    std::unique_lock lock{mutex_};
    queue_.clear();
  }
};

} // namespace v1

} // namespace ts

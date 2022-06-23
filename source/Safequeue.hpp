#pragma once

#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>
#include <shared_mutex>

template<typename T, std::size_t MaxSize>
requires((MaxSize > 0) && (MaxSize <= 8192)) class Safequeue final {
  std::queue<T>             queue_;
  mutable std::shared_mutex mutex_;

public:
  Safequeue() = default;

  Safequeue(Safequeue& other) noexcept      = delete;
  Safequeue& operator=(Safequeue&) noexcept = delete;

  [[nodiscard]] static constexpr std::size_t max_size() {
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

    queue_.push(std::forward<U>(element));

    return true;
  }

  [[nodiscard]] std::optional<T> pop() {
    std::unique_lock lock{mutex_};
    std::optional<T> result;

    if (queue_.empty()) {
      return result;
    }

    result = std::move(queue_.front());
    queue_.pop();

    return result;
  }
};

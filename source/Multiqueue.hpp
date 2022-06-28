#pragma once

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>

#include "Safequeue.hpp"

template<typename T, std::size_t MaxQueueSize>
class Multiqueue final {
  using Queue     = Safequeue<T, MaxQueueSize>;
  using Queues    = std::deque<Queue>;
  using QueueIter = typename Queues::iterator;

  static constexpr std::size_t MAX_NUMBER_OF_QUEUES = 1024;

  Queues    queues_;
  QueueIter sink_cursor_;

  void advance_sink_cursor() {
    if (++sink_cursor_ == queues_.end()) {
      sink_cursor_ = queues_.begin();
    }
  }

  [[nodiscard]] bool is_current_sink_full() {
    return (sink_cursor_->size() >= MaxQueueSize);
  }

  [[nodiscard]] bool advance_sink() {
    std::size_t advance_count = 0;
    do {
      advance_sink_cursor();
    } while (is_current_sink_full() && (advance_count++ < queues_.size()));

    return (advance_count < queues_.size());
  }

public:
  Multiqueue(std::size_t num_queues) {
    if (num_queues == 0) {
      throw std::underflow_error("Number of queues must be non-zero");
    }

    if (num_queues > MAX_NUMBER_OF_QUEUES) {
      throw std::overflow_error("Number of queues must be <" + std::to_string(MAX_NUMBER_OF_QUEUES));
    }

    queues_.resize(num_queues);

    sink_cursor_ = std::prev(queues_.end());
  }

  Multiqueue(Multiqueue&&) noexcept            = default;
  Multiqueue& operator=(Multiqueue&&) noexcept = default;

  [[nodiscard]] static constexpr std::size_t max_queue_size() {
    return MaxQueueSize;
  }

  [[nodiscard]] constexpr std::size_t num_queues() const {
    return queues_.size();
  }

  [[nodiscard]] constexpr std::size_t max_capacity() const {
    return queues_.size() * MaxQueueSize;
  }

  [[nodiscard]] bool empty() const {
    return std::all_of(queues_.begin(), queues_.end(), [](const auto& queue) { return queue.empty(); });
  }

  template<typename U>
  [[nodiscard]] bool push(U&& element) {
    if (!advance_sink()) {
      return false;
    }

    return sink_cursor_->push(std::forward<U>(element));
  }

  [[nodiscard]] std::optional<T> pop(std::size_t index) {
    if (index >= queues_.size()) {
      throw std::out_of_range("Queue index out of range");
    }

    QueueIter source = queues_.begin() + static_cast<typename Queues::difference_type>(index);

    if (source->empty()) {
      std::size_t advance_count = 0;
      do {
        source++;

        if (source == queues_.end()) {
          source = queues_.begin();
        }
      } while (source->empty() && (advance_count++ < queues_.size()));
    }

    return source->pop();
  }
};

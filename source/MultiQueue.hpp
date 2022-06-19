#pragma once

#include <cstddef>
#include <queue>
#include <stdexcept>
#include <string>

template<typename T, std::size_t MaxQueueLength>
requires(MaxQueueLength <= 8192) class MultiQueue final {
  using Queue     = std::queue<T>;
  using Queues    = std::vector<Queue>;
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
    return (sink_cursor_->size() >= MaxQueueLength);
  }

  [[nodiscard]] bool advance_sink() {
    unsigned int advance_count = 0;
    do {
      advance_sink_cursor();
    } while (is_current_sink_full() && (advance_count++ < queues_.size()));

    return (advance_count < queues_.size());
  }

public:
  MultiQueue(unsigned int num_queues) {
    if (num_queues == 0) {
      throw std::underflow_error("Number of queues must be non-zero");
    }

    if (num_queues > MAX_NUMBER_OF_QUEUES) {
      throw std::overflow_error("Number of queues must be <" + std::to_string(MAX_NUMBER_OF_QUEUES));
    }

    queues_.resize(num_queues);
    sink_cursor_ = queues_.begin();
  }

  [[nodiscard]] constexpr std::size_t max_queue_length() const {
    return MaxQueueLength;
  }

  [[nodiscard]] constexpr std::size_t max_capacity() const {
    return queues_.size() * MaxQueueLength;
  }

  [[nodiscard]] bool push(T&& element) {
    if (!advance_sink()) {
      return false;
    }

    sink_cursor_->push(std::forward<T>(element));

    return true;
  }

  MultiQueue(MultiQueue&&)            = default;
  MultiQueue& operator=(MultiQueue&&) = default;
};

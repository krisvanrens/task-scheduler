#pragma once

#include "Safequeue.hpp"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>

template<typename T, std::size_t MaxQueueSize>
class Multiqueue final {
  using Queue     = Safequeue<T, MaxQueueSize>;
  using Queues    = std::deque<Queue>;
  using QueueIter = typename Queues::iterator;

  static constexpr std::size_t MAX_NUMBER_OF_QUEUES = 1024;

  Queues    queues_;
  QueueIter sink_cursor_;

  void advance_sink_cursor();
  [[nodiscard]] bool is_current_sink_full();
  [[nodiscard]] bool advance_sink();

public:
  Multiqueue(unsigned int num_queues);

  Multiqueue(Multiqueue&&) noexcept            = default;
  Multiqueue& operator=(Multiqueue&&) noexcept = default;

  [[nodiscard]] static constexpr std::size_t max_queue_size();
  [[nodiscard]] constexpr std::size_t max_capacity() const;

  template<typename U>
  [[nodiscard]] bool push(U&& element);

  [[nodiscard]] std::optional<T> pop(unsigned int index);
};

template<typename T, std::size_t MaxQueueSize>
void Multiqueue<T, MaxQueueSize>::advance_sink_cursor() {
  if (++sink_cursor_ == queues_.end()) {
    sink_cursor_ = queues_.begin();
  }
}

template<typename T, std::size_t MaxQueueSize>
bool Multiqueue<T, MaxQueueSize>::is_current_sink_full() {
  return (sink_cursor_->size() >= MaxQueueSize);
}

template<typename T, std::size_t MaxQueueSize>
bool Multiqueue<T, MaxQueueSize>::advance_sink() {
  unsigned int advance_count = 0;
  do {
    advance_sink_cursor();
  } while (is_current_sink_full() && (advance_count++ < queues_.size()));

  return (advance_count < queues_.size());
}

template<typename T, std::size_t MaxQueueSize>
Multiqueue<T, MaxQueueSize>::Multiqueue(unsigned int num_queues) {
  if (num_queues == 0) {
    throw std::underflow_error("Number of queues must be non-zero");
  }

  if (num_queues > MAX_NUMBER_OF_QUEUES) {
    throw std::overflow_error("Number of queues must be <" + std::to_string(MAX_NUMBER_OF_QUEUES));
  }

  queues_.resize(num_queues);

  sink_cursor_ = std::prev(queues_.end());
}

template<typename T, std::size_t MaxQueueSize>
constexpr std::size_t Multiqueue<T, MaxQueueSize>::max_queue_size() {
  return MaxQueueSize;
}

template<typename T, std::size_t MaxQueueSize>
constexpr std::size_t Multiqueue<T, MaxQueueSize>::max_capacity() const {
  return queues_.size() * MaxQueueSize;
}

template<typename T, std::size_t MaxQueueSize>
template<typename U>
bool Multiqueue<T, MaxQueueSize>::push(U&& element) {
  if (!advance_sink()) {
    return false;
  }

  return sink_cursor_->push(std::forward<U>(element));
}

template<typename T, std::size_t MaxQueueSize>
std::optional<T> Multiqueue<T, MaxQueueSize>::pop(unsigned int index) {
  if (index >= queues_.size()) {
    throw std::out_of_range("Queue index out of range");
  }

  QueueIter source = queues_.begin() + index;

  if (source->empty()) {
    unsigned int advance_count = 0;
    do {
      source++;

      if (source == queues_.end()) {
        source = queues_.begin();
      }
    } while (source->empty() && (advance_count++ < queues_.size()));
  }

  return source->pop();
}

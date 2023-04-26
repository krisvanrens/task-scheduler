#pragma once

#include <algorithm>
#include <cstddef>
#include <deque>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>

#include "safe_queue.hpp"

namespace ts {

inline namespace v1 {

/// Array of thread-safe queues. This type features an API similar to a single queue. For any item push, the load is
///  uniformly distributed over the internal queues. The pop call is called with an index to indicate the internal queue
///  index. However, when the indexed queue is empty, data is 'stolen' from the next non-empty queue.
template<typename T, std::size_t MaxQueueSize>
class multiqueue final {
  using queue_t    = safe_queue<T, MaxQueueSize>;
  using queues     = std::deque<queue_t>;
  using queue_iter = typename queues::iterator;

  static constexpr std::size_t MAX_NUMBER_OF_QUEUES{1024};

  queues     queues_;
  queue_iter sink_cursor_;

  void advance_sink_cursor() {
    if (++sink_cursor_ == queues_.end()) {
      sink_cursor_ = queues_.begin();
    }
  }

  [[nodiscard]] bool is_current_sink_full() const {
    return (sink_cursor_->size() >= MaxQueueSize);
  }

  [[nodiscard]] bool advance_sink() {
    std::size_t advance_count{};
    do {
      advance_sink_cursor();
    } while (is_current_sink_full() && (advance_count++ < queues_.size()));

    return (advance_count < queues_.size());
  }

public:
  explicit multiqueue(std::size_t num_queues) {
    if (num_queues == 0) {
      throw std::underflow_error("Number of queues must be non-zero");
    }

    if (num_queues > MAX_NUMBER_OF_QUEUES) {
      throw std::overflow_error("Number of queues must be <" + std::to_string(MAX_NUMBER_OF_QUEUES));
    }

    queues_.resize(num_queues);

    sink_cursor_ = std::prev(queues_.end());
  }

  multiqueue(multiqueue&&) noexcept            = default;
  multiqueue& operator=(multiqueue&&) noexcept = default;

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

  [[nodiscard]] std::size_t size() const {
    return std::accumulate(queues_.begin(), queues_.end(), 0u,
                           [](const auto& size, const auto& queue) { return (size + queue.size()); });
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

    queue_iter source{queues_.begin() + static_cast<typename queues::difference_type>(index)};

    if (source->empty()) {
      std::size_t advance_count{};
      do {
        source++;

        if (source == queues_.end()) {
          source = queues_.begin();
        }
      } while (source->empty() && (advance_count++ < queues_.size()));
    }

    return source->pop();
  }

  void flush() {
    for (auto& queue : queues_) {
      queue.flush();
    }
  }
};

} // namespace v1

} // namespace ts

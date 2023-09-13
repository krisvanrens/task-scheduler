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

///
/// Array of thread-safe queues (FIFO) with a single interface.
///
/// This type features an API similar to a single queue. For any item push, the load is uniformly distributed over the
///  internal queues. The pop call is called with an index to indicate the internal queue index. However, when the in-
///  dexed queue is empty, data is 'stolen' from the next non-empty queue (work stealing).
///
/// \param T            The queue element value type.
/// \param MaxQueueSize The maximum queue size. Must be in range `1..MAX_SIZE_LIMIT` (limit for a single queue).
///
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
  ///
  /// Constructor.
  ///
  /// \param num_queues The number of underlying queues to instantiate.
  ///
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

  ///
  /// Get the maximum queue size.
  ///
  /// \returns The maximum queue size.
  ///
  [[nodiscard]] static constexpr std::size_t max_queue_size() noexcept {
    return MaxQueueSize;
  }

  ///
  /// Get the number of underlying queues.
  ///
  /// \returns The number of underlying queues.
  ///
  [[nodiscard]] constexpr std::size_t num_queues() const noexcept {
    return queues_.size();
  }

  ///
  /// Get the maximum total capacity: the single queue capacity accumulated for all queues.
  ///
  /// \returns The total multiqueue capacity.
  ///
  [[nodiscard]] constexpr std::size_t max_capacity() const noexcept {
    return queues_.size() * MaxQueueSize;
  }

  ///
  /// Check if the queue is empty.
  ///
  /// \returns `true` if the queue is empty, `false` if otherwise.
  ///
  [[nodiscard]] bool empty() const noexcept {
    return std::all_of(queues_.begin(), queues_.end(), [](const auto& queue) { return queue.empty(); });
  }

  ///
  /// Get the current queue occupation size.
  ///
  /// \returns The number of elements in the queue.
  ///
  [[nodiscard]] std::size_t size() const noexcept {
    return std::accumulate(queues_.begin(), queues_.end(), 0u,
                           [](const auto& size, const auto& queue) { return (size + queue.size()); });
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
    if (!advance_sink()) {
      return false;
    }

    return sink_cursor_->push(std::forward<U>(element));
  }

  ///
  /// Pop an element off the front of an underlying queue. Will employ work stealing to select a non-empty queue.
  ///
  /// \param index Underlying queue index to pop from. If the indexed queue is empty, other queues will be checked in
  ///               a round-robin style to steal work.
  ///
  /// \returns An optional element. The optional is empty if all queues were empty.
  ///
  /// \throws `std::out_of_range` if the queue index is out of range.
  ///
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

  ///
  /// Flush the queue, removing all elements from all queues.
  ///
  void flush() {
    for (auto& queue : queues_) {
      queue.flush();
    }
  }
};

} // namespace v1

} // namespace ts

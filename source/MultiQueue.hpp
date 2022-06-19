#pragma once

#include <cstddef>
#include <queue>
#include <stdexcept>

constexpr std::size_t MAX_NUMBER_OF_QUEUES = 1024;
constexpr std::size_t MAX_QUEUE_LENGTH     = 8192;

template<typename T, std::size_t MaxQueueLength>
requires(MaxQueueLength < MAX_QUEUE_LENGTH) class MultiQueue final {
  using Queue = std::queue<T>;

  std::vector<Queue> queues_;

public:
  MultiQueue(unsigned int num_queues) {
    if (num_queues == 0) {
      throw std::underflow_error("Number of queues must be non-zero");
    }

    if (num_queues > MAX_NUMBER_OF_QUEUES) {
      throw std::overflow_error("Too many queues requested");
    }

    queues_.resize(num_queues);
  }

  MultiQueue(MultiQueue&&)            = default;
  MultiQueue& operator=(MultiQueue&&) = default;
};

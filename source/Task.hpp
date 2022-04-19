#pragma once

#include <memory>
#include <utility>

class Task final {
  struct Concept {
    virtual ~Concept() = default;
  };

  template<typename T>
  struct Model final : Concept {
    template<typename U = T>
    Model(U&& value)
      : value_{std::forward<U>(value)} {
    }

    const T value_;
  };

  std::unique_ptr<const Concept> model_;

public:
  constexpr Task() = default;

  template<typename T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, Task>, bool> = true>
  Task(T&& value)
    : model_{std::make_unique<Model<std::decay_t<T>>>(std::forward<T>(value))} {
  }

  Task(Task&&) noexcept = default;
  Task& operator=(Task&&) noexcept = default;
};

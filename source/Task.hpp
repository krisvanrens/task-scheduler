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

  const std::shared_ptr<const Concept> model_;

public:
  template<typename T>
  Task(T&& value)
    : model_{std::make_shared<Model<std::decay_t<T>>>(std::forward<T>(value))} {
  }
};

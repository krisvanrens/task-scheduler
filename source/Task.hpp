#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

template<typename>
class Task;

template<typename Ret, typename... Args>
class Task<Ret(Args...)> final {
  template<typename T>
  using is_task_t = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Task>, bool>;

  struct Concept {
    virtual ~Concept()           = default;
    virtual Ret invoke_(Args...) = 0;
  };

  template<typename T>
  struct Model final : Concept {
    template<typename U = T>
    Model(U&& value)
      : value_{std::forward<U>(value)} {
    }

    Ret invoke_(Args... args) override {
      return std::invoke(value_, std::forward<Args>(args)...);
    }

    T value_;
  };

  std::unique_ptr<Concept> model_;

public:
  constexpr Task() = default;

  template<typename T, typename = is_task_t<T>>
  requires std::is_invocable_r_v<Ret, T, Args...> Task(T&& value)
    : model_{std::make_unique<Model<std::decay_t<T>>>(std::forward<T>(value))} {
  }

  Task(Task&&) noexcept            = default;
  Task& operator=(Task&&) noexcept = default;

  explicit operator bool() const noexcept {
    return !!model_;
  }

  Ret operator()(Args... args) {
    if (!model_) {
      throw std::bad_function_call{};
    }

    return model_->invoke_(std::forward<Args>(args)...);
  }
};

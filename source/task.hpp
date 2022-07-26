#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace ts {

inline namespace v1 {

template<typename>
class task;

template<typename Ret, typename... Args>
class task<Ret(Args...)> final {
  struct concept_t {
    virtual ~concept_t()         = default;
    virtual Ret invoke_(Args...) = 0;
  };

  template<typename T>
  struct model_t final : concept_t {
    template<typename U = T>
    explicit model_t(U&& value)
      : value_{std::forward<U>(value)} {
    }

    Ret invoke_(Args... args) override {
      return std::invoke(value_, std::forward<Args>(args)...);
    }

    T value_;
  };

  std::unique_ptr<concept_t> model_;

public:
  constexpr task() = default;

  template<typename T>
  requires(std::is_invocable_r_v<Ret, T, Args...> && !std::same_as<std::decay_t<T>, task>) task(T&& value)
    : model_{std::make_unique<model_t<std::decay_t<T>>>(std::forward<T>(value))} {
  }

  task(task&&) noexcept            = default;
  task& operator=(task&&) noexcept = default;

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

} // namespace v1

} // namespace ts

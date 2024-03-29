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

///
/// Task wrapper to hold type-erased callables like function objects and function pointers.
///
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

  ///
  /// Constructor.
  ///
  /// \param value The callable object to wrap.
  ///
  template<typename T>
  requires(std::is_invocable_r_v<Ret, T, Args...> && !std::same_as<std::decay_t<T>, task>) task(T&& value)
    : model_{std::make_unique<model_t<std::decay_t<T>>>(std::forward<T>(value))} {
  }

  task(task&&) noexcept            = default;
  task& operator=(task&&) noexcept = default;

  ///
  /// Check task validity.
  ///
  /// \returns `true` if the task holds a callable object, `false` if otherwise.
  ///
  explicit operator bool() const noexcept {
    return !!model_;
  }

  ///
  /// Call the underlying callable object.
  ///
  /// \param args Parameter pack with callable arguments.
  ///
  /// \returns The return value of the callable.
  ///
  /// \throws `std::bad_function_call` if the wrapped callable object is empty.
  ///
  Ret operator()(Args... args) {
    if (!model_) {
      throw std::bad_function_call{};
    }

    return model_->invoke_(std::forward<Args>(args)...);
  }
};

} // namespace v1

} // namespace ts

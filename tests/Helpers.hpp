#pragma once

#include "Tracer.hpp"

class Callable final : public Tracer {
  bool& is_called_;

public:
  explicit Callable(bool& is_called)
    : Tracer{}
    , is_called_{is_called} {
  }

  void operator()() {
    is_called_ = true;
  }
};

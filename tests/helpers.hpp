#pragma once

#include "tracer.hpp"

namespace helpers {

class callable final : public tracer {
  bool& is_called_;

public:
  explicit callable(bool& is_called)
    : tracer{}
    , is_called_{is_called} {
  }

  void operator()() {
    is_called_ = true;
  }
};

} // namespace helpers

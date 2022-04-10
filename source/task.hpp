#pragma once

#include <memory>
#include <utility>

class Task {
  struct Concept {
    virtual ~Concept = default;
  };

  const std::shared_ptr<const Concept> model_;

public:
};

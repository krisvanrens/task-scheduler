#pragma once

#include <cstdio>

class tracer {
  bool verbose_ = false;

  struct counts {
    unsigned int ctor        = 0;
    unsigned int dtor        = 0;
    unsigned int copy_ctor   = 0;
    unsigned int copy_assign = 0;
    unsigned int move_ctor   = 0;
    unsigned int move_assign = 0;

    void print() const {
      printf("counts: [ctor=%u, dtor=%u, copy_ctor=%u, copy_assign=%u, move_ctor=%u, move_assign=%u]\n", ctor, dtor,
             copy_ctor, copy_assign, move_ctor, move_assign);
    }
  } counts_;

public:
  tracer(bool verbose = false)
    : verbose_{verbose} {
    counts_.ctor++;

    if (verbose_) {
      printf("%p: ctor\n", static_cast<void*>(this));
    }
  }

  ~tracer() {
    counts_.dtor++;

    if (verbose_) {
      printf("%p: dtor\n", static_cast<void*>(this));
    }
  }

  tracer(const tracer& other)
    : verbose_{other.verbose_}
    , counts_{other.counts_} {
    counts_.copy_ctor++;

    if (verbose_) {
      printf("%p: copy ctor\n", static_cast<void*>(this));
    }
  }

  tracer& operator=(const tracer& other) {
    verbose_ = other.verbose_;
    counts_  = other.counts_;

    counts_.copy_assign++;

    if (verbose_) {
      printf("%p: copy assign\n", static_cast<void*>(this));
    }

    return *this;
  }

  tracer(tracer&& other)
    : verbose_{other.verbose_}
    , counts_{other.counts_} {
    counts_.move_ctor++;

    if (verbose_) {
      printf("%p: move ctor\n", static_cast<void*>(this));
    }
  }

  tracer& operator=(tracer&& other) {
    verbose_ = other.verbose_;
    counts_  = other.counts_;

    counts_.move_assign++;

    if (verbose_) {
      printf("%p: move assign\n", static_cast<void*>(this));
    }

    return *this;
  }

  void set_verbose(bool verbose) {
    verbose_ = verbose;
  }

  const counts& counts() const {
    return counts_;
  }
};

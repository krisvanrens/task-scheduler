#pragma once

#include <cstdio>

class Tracer {
  bool verbose_ = false;

  struct Counts {
    unsigned int ctor        = 0;
    unsigned int dtor        = 0;
    unsigned int copy_ctor   = 0;
    unsigned int copy_assign = 0;
    unsigned int move_ctor   = 0;
    unsigned int move_assign = 0;

    void print() const {
      printf("Counts: [ctor=%u, dtor=%u, copy_ctor=%u, copy_assign=%u, move_ctor=%u, move_assign=%u]\n", ctor, dtor,
             copy_ctor, copy_assign, move_ctor, move_assign);
    }
  } counts_;

public:
  Tracer(bool verbose = false)
    : verbose_{verbose} {
    counts_.ctor++;

    if (verbose_) {
      puts("ctor");
    }
  }

  ~Tracer() {
    counts_.dtor++;

    if (verbose_) {
      puts("dtor");
    }
  }

  Tracer(const Tracer& other) {
    verbose_ = other.verbose_;
    counts_  = other.counts_;

    counts_.copy_ctor++;

    if (verbose_) {
      puts("copy ctor");
    }
  }

  Tracer& operator=(const Tracer& other) {
    verbose_ = other.verbose_;
    counts_  = other.counts_;

    counts_.copy_assign++;

    if (verbose_) {
      puts("copy assign");
    }

    return *this;
  }

  Tracer(Tracer&& other) {
    verbose_ = other.verbose_;
    counts_  = other.counts_;

    counts_.move_ctor++;

    if (verbose_) {
      puts("move ctor");
    }
  }

  Tracer& operator=(Tracer&& other) {
    verbose_ = other.verbose_;
    counts_  = other.counts_;

    counts_.move_assign++;

    if (verbose_) {
      puts("move assign");
    }

    return *this;
  }

  void set_verbose(bool verbose) {
    verbose_ = verbose;
  }

  const Counts& counts() const {
    return counts_;
  }
};

# task-scheduler

Task Scheduler in C++.

# Requirements

Requirements levels as defined in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

## MUST

* Support running tasks based on:
  * Class member functions,
  * Free functions,
  * Lambda expressions.
* Support a configurable scheduling strategy.
* Support the configuration of the amount of workers.

## SHOULD

* Support the concept of task priority.
* Support scheduling of automatic periodic tasks.

## MAY

* Support small object optimization for task objects.

## Building

## Requirements

* Conan (for building the tests)
* GCC v10 or higher, or Clang v12 or higher
* Clang-format v12 or higher

## Instructions

```sh
mkdir build
ccmake -S . -B ./build/
cd build
make
```

# task-scheduler

Task Scheduler in C++20.

## Building

Requirements:

* Conan (for building the tests),
* GCC v12 or higher, or Clang v13 or higher,
* Clang-format v13 or higher,

Instructions:

```sh
mkdir build
ccmake -S . -B ./build/
cd build
make
```

# Functional requirements

Aside from the common-sense requirements for correct, efficient and thread-safe implementation, there are functional requirements.
The following requirements levels are as defined in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

## MUST

* Support running tasks based on:
  * Class member functions,
  * Free functions,
  * Lambda expressions.
* Support a configurable scheduling strategy. (TODO)
* Support the configuration of the amount of workers.

## SHOULD

* Support the concept of task priority. (TODO)
* Support scheduling of automatic periodic tasks. (TODO)

## MAY

* Support small object optimization for task objects. (TODO)

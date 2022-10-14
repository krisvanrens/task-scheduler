# task-scheduler

Task Scheduler in C++20.

## Building

Requirements:

* Conan (for building the tests),
* GCC v12 or higher, or Clang v13 or higher,
* Clang-format v14 or higher,

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
* Support the configuration of the amount of workers.
* Support the configuration of the work queue length.
* Support deferred return value propagation for task results. (TODO)
* Support the graceful handling of exceptions (TODO)

## SHOULD

* Support the concept of task priority. (TODO)
* Support scheduling of automatic periodic tasks. (TODO)
* Support a configurable scheduling strategy. (TODO)
* Support a mode that keeps a strict task order. (TODO)

## MAY

* Support small object optimization for task objects. (TODO)

# Ideas

* Use `if constexpr (std::is_void_v<T>)` to detect tasks without a return value.

# License

This project is licensed under the [Boost Software License 1.0](https://choosealicense.com/licenses/bsl-1.0/).

add_executable(constexpr_tests constexpr_tests.cpp)
target_link_libraries(
  constexpr_tests
  PRIVATE
  project_options
  project_warnings
  CONAN_PKG::doctest)

add_executable(relaxed_constexpr_tests constexpr_tests.cpp)
target_link_libraries(
  relaxed_constexpr_tests
  PRIVATE
  project_options
  project_warnings
  CONAN_PKG::doctest)

target_compile_definitions(relaxed_constexpr_tests PRIVATE -DTEST_NONSTATIC_REQUIRE)

add_executable(tests_completion_token completion_token.cpp)
target_link_libraries(
  tests_completion_token
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_multiqueue multiqueue.cpp)
target_link_libraries(
  tests_multiqueue
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_safe_queue safe_queue.cpp)
target_link_libraries(
  tests_safe_queue
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_simple_scheduler simple_scheduler.cpp)
target_link_libraries(
  tests_simple_scheduler
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_task task.cpp)
target_link_libraries(
  tests_task
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_tracer tracer.cpp)
target_link_libraries(
  tests_tracer
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

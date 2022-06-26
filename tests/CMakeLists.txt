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

add_executable(tests_Multiqueue Multiqueue.cpp)
target_link_libraries(
  tests_Multiqueue
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_Safequeue Safequeue.cpp)
target_link_libraries(
  tests_Safequeue
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_SimpleScheduler SimpleScheduler.cpp)
target_link_libraries(
  tests_SimpleScheduler
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_Task Task.cpp)
target_link_libraries(
  tests_Task
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

add_executable(tests_Tracer Tracer.cpp)
target_link_libraries(
  tests_Tracer
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::doctest)

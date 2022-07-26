add_executable(benches_safe_queue safe_queue.cpp)
target_link_libraries(
  benches_safe_queue
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::benchmark)

add_executable(benches_simple_scheduler simple_scheduler.cpp)
target_link_libraries(
  benches_simple_scheduler
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::benchmark)

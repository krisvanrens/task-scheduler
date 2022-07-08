add_executable(benches_Safequeue Safequeue.cpp)
target_link_libraries(
  benches_Safequeue
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::benchmark)

add_executable(benches_SimpleScheduler SimpleScheduler.cpp)
target_link_libraries(
  benches_SimpleScheduler
  PRIVATE
  project_warnings
  project_options
  CONAN_PKG::benchmark)

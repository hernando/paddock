add_executable(utils_tests)

target_sources(utils_tests
  PRIVATE
    encodings.cpp
)

target_link_libraries(utils_tests PRIVATE
  gtest_main
  paddock::utils
)

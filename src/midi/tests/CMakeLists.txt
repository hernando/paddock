add_executable(midi_tests)

target_sources(midi_tests
  PRIVATE
    sceneEncoding.cpp
)

target_link_libraries(midi_tests PRIVATE
  gtest_main
  paddock::midi
)

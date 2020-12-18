add_library(paddock_utils)
add_library(paddock::utils ALIAS paddock_utils)

target_sources(paddock_utils
PUBLIC
  Expected.hpp
  byte.hpp
  mp.hpp
  overloaded.hpp
PRIVATE
  encodings.cpp
)

add_subdirectory(tests)
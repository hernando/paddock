add_library(paddock_utils INTERFACE)
add_library(paddock::utils ALIAS paddock_utils)

target_sources(paddock_utils INTERFACE
  Expected.hpp
  byte.hpp
  overloaded.hpp
)


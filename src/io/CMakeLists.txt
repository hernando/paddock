add_library(paddock_io INTERFACE)
add_library(paddock::io ALIAS paddock_io)

target_sources(paddock_io
  INTERFACE
    Serializer.hpp
)

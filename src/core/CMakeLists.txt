add_library(paddock_core STATIC)
add_library(paddock::core ALIAS paddock_core)

target_sources(paddock_core PRIVATE
  Session.hpp
  Session.cpp
)

if(PADDOCK_USE_LIBLO)
  target_link_libraries(paddock_core
    PRIVATE
      lo
  )
endif()



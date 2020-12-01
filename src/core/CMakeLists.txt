add_library(paddock_core STATIC)
add_library(paddock::core ALIAS paddock_core)

target_sources(paddock_core
  PUBLIC
    Globals.hpp
    Log.hpp
    Poller.hpp
    Program.hpp
    errors.hpp
    resources.hpp
  PRIVATE
    Globals.cpp
    Log.cpp
    Poller.cpp
    Program.cpp
    errors.cpp
    resources.cpp

    platform/poll.hpp
    platform/poll.cpp
    platform/posix/poll.hpp
)

target_link_libraries(paddock_core
  PRIVATE
    Qt5::Core
)

if(PADDOCK_USE_LIBLO)
  target_sources(paddock_core PRIVATE
    NsmSession.hpp
    NsmSession.cpp
  )

  target_link_libraries(paddock_core
    PRIVATE
      paddock::utils
      lo
  )
endif()

set_property(TARGET paddock_core PROPERTY AUTOMOC TRUE)
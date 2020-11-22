add_library(paddock_core STATIC)
add_library(paddock::core ALIAS paddock_core)

target_sources(paddock_core PRIVATE
  Globals.hpp
  Globals.cpp
  Log.hpp
  Log.cpp
  Program.cpp
  Program.hpp
  errors.hpp
  errors.cpp
  resources.hpp
  resources.cpp
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
      lo
  )
endif()

set_property(TARGET paddock_core PROPERTY AUTOMOC TRUE)
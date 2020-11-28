add_library(paddock_midi STATIC)
add_library(paddock::midi ALIAS paddock_midi)

target_sources(paddock_midi
  PUBLIC
    Client.hpp
    Device.hpp
    Engine.hpp
    Events.hpp
  PRIVATE
    Client.cpp
    ClientPrivate.hpp
    Device.cpp
    Engine.cpp
)

target_link_libraries(paddock_midi PRIVATE
  paddock::utils
  Threads::Threads
)

if(PADDOCK_USE_ALSA)
  target_sources(paddock_midi PRIVATE
    platform/alsa/Sequencer.hpp
    platform/alsa/Sequencer.cpp
  )

  target_link_libraries(paddock_midi
    PRIVATE
      asound
  )
endif()


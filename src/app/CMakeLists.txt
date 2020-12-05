add_executable(paddock)

target_sources(paddock PRIVATE
  Session.cpp
  Session.hpp
  resources.hpp
  resources.cpp
  utils.hpp

  pads/KorgPadKontrol.hpp
  pads/KorgPadKontrol.cpp
  pads/models.hpp
  pads/pads.hpp
  pads/pads.cpp

  main.cpp
)

target_include_directories(paddock PRIVATE
  ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(
    paddock
  PRIVATE
    paddock::core
    paddock::midi
    paddock::ui
    Qt5::Core
    Qt5::Quick
)

set_property(TARGET paddock PROPERTY AUTOMOC TRUE)
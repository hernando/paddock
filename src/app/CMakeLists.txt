add_executable(paddock)

target_sources(paddock PRIVATE
  Program.cpp
  Program.hpp
  Session.cpp
  Session.hpp
  resources.hpp
  resources.cpp
  utils.hpp

  pads/KorgPadKontrol.hpp
  pads/KorgPadKontrol.cpp
  pads/korgPadKontrol/KnobController.hpp
  pads/korgPadKontrol/KnobController.cpp
  pads/korgPadKontrol/KnobModel.hpp
  pads/korgPadKontrol/KnobModel.cpp
  pads/korgPadKontrol/Program.hpp
  pads/korgPadKontrol/Program.cpp
  pads/korgPadKontrol/TriggerController.hpp
  pads/korgPadKontrol/TriggerController.cpp
  pads/korgPadKontrol/TriggerModel.hpp
  pads/korgPadKontrol/TriggerModel.cpp
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
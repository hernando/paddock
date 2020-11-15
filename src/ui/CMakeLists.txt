add_library(paddock_ui STATIC)
add_library(paddock::ui ALIAS paddock_ui)

qtquick_compiler_add_resources(resources qml/resources.qrc)

target_sources(paddock_ui PRIVATE
  Styling.hpp
  Styling.cpp
  module.hpp
  module.cpp
  resources.cpp
  resources.hpp
  ${resources}
)

target_link_libraries(paddock_ui
  PUBLIC
    Qt5::Core
    Qt5::Quick
    Qt5::QuickControls2
)

set_property(TARGET paddock_ui PROPERTY AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

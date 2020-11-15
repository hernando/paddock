cmake_minimum_required(VERSION 3.14)

project(Paddock)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(BaseConfig)
include(FindDependencies)

add_subdirectory(src/core)
add_subdirectory(src/ui)

add_executable(paddock src/main.cpp)
target_link_libraries(
    paddock
  PRIVATE
    paddock::ui
    paddock::core
    Qt5::Core Qt5::Quick
)





cmake_minimum_required(VERSION 3.14)

project(Paddock)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(BaseConfig)
include(FindDependencies)

include_directories(ext/expected/include)
include_directories(src)

add_subdirectory(src/utils)

add_subdirectory(src/core)
add_subdirectory(src/midi)
add_subdirectory(src/ui)

add_subdirectory(src/app)

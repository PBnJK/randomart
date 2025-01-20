cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CONFIGURATION_TYPES Debug;Release;RelWithDebInfo)

project(randomart)

include_directories(inc)
file(GLOB SOURCES src/*.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build_wasm")
add_executable(randomart ${SOURCES})

message(STATUS "Setting compilation target to WASM")
set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")

target_compile_options(randomart PRIVATE -g)
target_link_options(randomart PRIVATE "-sWASM=1")
target_link_options(randomart PRIVATE "-sEXPORTED_FUNCTIONS=[\"_main\"]")

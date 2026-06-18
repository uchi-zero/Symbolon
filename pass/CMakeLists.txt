cmake_minimum_required(VERSION 3.20)
project(symbolon-passes C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)

find_package(LLVM 16 REQUIRED CONFIG)
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")

CPMAddPackage(
  NAME nlohmann_json
  GITHUB_REPOSITORY nlohmann/json
  VERSION 3.11.3
  OPTIONS "JSON_BuildTests OFF"
)

llvm_map_components_to_libnames(LLVM_LIBS
  Core IRReader Support Passes Analysis BitReader)

enable_testing()
add_subdirectory(common)
add_subdirectory(fork-finder)
add_subdirectory(line-counter)
add_subdirectory(branch-counter)
add_subdirectory(state-killer-finder)

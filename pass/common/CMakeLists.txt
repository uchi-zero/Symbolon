add_library(SymbolonCommon STATIC src/Paths.cpp)
target_include_directories(SymbolonCommon
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

CPMAddPackage(
  NAME cxxopts
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 3.2.0
  OPTIONS "CXXOPTS_BUILD_EXAMPLES OFF" "CXXOPTS_BUILD_TESTS OFF"
)

add_executable(pass-fixture-runner src/FixtureRunner.cpp)
target_link_libraries(pass-fixture-runner
  PRIVATE nlohmann_json::nlohmann_json cxxopts::cxxopts ${LLVM_LIBS})
target_compile_definitions(pass-fixture-runner PRIVATE ${LLVM_DEFINITIONS})
target_include_directories(pass-fixture-runner PRIVATE ${LLVM_INCLUDE_DIRS})

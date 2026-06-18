add_library(StateKillerFinderCore STATIC
  src/StateKillerFinderPass.cpp
  src/StateKillerFinder.cpp
  src/Categories.cpp
  src/Report.cpp)
target_include_directories(StateKillerFinderCore
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})
target_compile_definitions(StateKillerFinderCore PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(StateKillerFinderCore
  PUBLIC SymbolonCommon nlohmann_json::nlohmann_json ${LLVM_LIBS})

add_executable(state-killer-finder src/Main.cpp)
target_link_libraries(state-killer-finder PRIVATE StateKillerFinderCore)

add_subdirectory(tests)

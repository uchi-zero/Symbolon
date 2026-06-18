add_library(ForkFinderCore STATIC
  src/ForkFinderPass.cpp
  src/ForkFinder.cpp
  src/Report.cpp
  src/Categories.cpp)
target_include_directories(ForkFinderCore
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})
target_compile_definitions(ForkFinderCore PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(ForkFinderCore
  PUBLIC SymbolonCommon nlohmann_json::nlohmann_json ${LLVM_LIBS})

add_executable(fork-finder src/Main.cpp)
target_link_libraries(fork-finder PRIVATE ForkFinderCore)

add_subdirectory(tests)

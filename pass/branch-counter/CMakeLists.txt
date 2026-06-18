add_library(BranchCounterCore STATIC src/BranchCounter.cpp)
target_include_directories(BranchCounterCore
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})
target_compile_definitions(BranchCounterCore PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(BranchCounterCore
  PUBLIC SymbolonCommon nlohmann_json::nlohmann_json ${LLVM_LIBS})

add_executable(branch-counter src/Main.cpp)
target_link_libraries(branch-counter PRIVATE BranchCounterCore)

add_subdirectory(tests)

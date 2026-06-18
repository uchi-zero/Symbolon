add_library(LineCounterCore STATIC src/LineCounter.cpp)
target_include_directories(LineCounterCore
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})
target_compile_definitions(LineCounterCore PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(LineCounterCore
  PUBLIC SymbolonCommon nlohmann_json::nlohmann_json ${LLVM_LIBS})

add_executable(line-counter src/Main.cpp)
target_link_libraries(line-counter PRIVATE LineCounterCore)

add_subdirectory(tests)

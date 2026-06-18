add_test(
  NAME state_killer_finder_version
  COMMAND $<TARGET_FILE:state-killer-finder> --version
)
set_tests_properties(state_killer_finder_version PROPERTIES
  PASS_REGULAR_EXPRESSION "state-killer-finder 0\\."
)

add_test(
  NAME state_killer_finder_fixtures
  COMMAND $<TARGET_FILE:pass-fixture-runner>
          --tool=state-killer-finder
          --binary=$<TARGET_FILE:state-killer-finder>
          --inputs=${CMAKE_CURRENT_SOURCE_DIR}/inputs
          --golden=${CMAKE_CURRENT_SOURCE_DIR}/golden
)

add_test(
  NAME state_killer_finder_missing_input
  COMMAND sh -c "$<TARGET_FILE:state-killer-finder> /no/such/file.bc; test $? -eq 1"
)

add_executable(skf_test_types unit/test_types.cpp)
target_link_libraries(skf_test_types PRIVATE StateKillerFinderCore)
add_test(NAME skf_test_types COMMAND skf_test_types)

add_executable(skf_test_report unit/test_report.cpp)
target_link_libraries(skf_test_report PRIVATE StateKillerFinderCore)
add_test(NAME skf_test_report COMMAND skf_test_report)

add_executable(skf_test_pretty unit/test_pretty.cpp)
target_link_libraries(skf_test_pretty PRIVATE StateKillerFinderCore)
add_test(NAME skf_test_pretty COMMAND skf_test_pretty)

add_executable(skf_test_categories unit/test_categories.cpp)
target_link_libraries(skf_test_categories PRIVATE StateKillerFinderCore)
add_test(NAME skf_test_categories COMMAND skf_test_categories)

# Compile the enable_all fixture to bitcode in the build tree so the CLI
# assertions below can invoke the pass binary on a stable path.
find_program(CLANG_EXECUTABLE clang REQUIRED)
set(SKF_ENABLE_ALL_BC ${CMAKE_CURRENT_BINARY_DIR}/enable_all.bc)
add_custom_command(
  OUTPUT ${SKF_ENABLE_ALL_BC}
  COMMAND ${CLANG_EXECUTABLE} -O0 -g -emit-llvm -c
          ${CMAKE_CURRENT_SOURCE_DIR}/inputs/enable_all.c
          -o ${SKF_ENABLE_ALL_BC}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/inputs/enable_all.c
  COMMENT "Compiling enable_all.c -> enable_all.bc"
)
add_custom_target(skf_enable_all_bc ALL DEPENDS ${SKF_ENABLE_ALL_BC})

add_test(
  NAME state_killer_finder_enable_all_lists_all_categories
  COMMAND sh -c "$<TARGET_FILE:state-killer-finder> --enable=all $<TARGET_FILE_DIR:state-killer-finder>/tests/enable_all.bc | grep -q '\"FreeOfNonHeap\"'"
)

add_test(
  NAME state_killer_finder_disable_removed
  COMMAND sh -c "$<TARGET_FILE:state-killer-finder> --disable=ProgramTerminator /dev/null; test $? -ne 0"
)

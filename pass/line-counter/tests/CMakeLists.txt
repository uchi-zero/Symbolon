add_test(
  NAME line_counter_version
  COMMAND $<TARGET_FILE:line-counter> --version
)
set_tests_properties(line_counter_version PROPERTIES
  PASS_REGULAR_EXPRESSION "line-counter 0\\."
)

add_test(
  NAME line_counter_fixtures
  COMMAND $<TARGET_FILE:pass-fixture-runner>
          --tool=line-counter
          --binary=$<TARGET_FILE:line-counter>
          --inputs=${CMAKE_CURRENT_SOURCE_DIR}/inputs
          --golden=${CMAKE_CURRENT_SOURCE_DIR}/golden
)

add_test(
  NAME line_counter_missing_input
  COMMAND sh -c "$<TARGET_FILE:line-counter> /no/such/file.bc; test $? -eq 1"
)

add_executable(line_counter_test_system_path unit/test_system_path.cpp)
target_link_libraries(line_counter_test_system_path PRIVATE SymbolonCommon)
add_test(NAME line_counter_test_system_path COMMAND line_counter_test_system_path)

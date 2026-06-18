add_test(
  NAME branch_counter_version
  COMMAND $<TARGET_FILE:branch-counter> --version
)
set_tests_properties(branch_counter_version PROPERTIES
  PASS_REGULAR_EXPRESSION "branch-counter 0\\."
)

add_test(
  NAME branch_counter_fixtures
  COMMAND $<TARGET_FILE:pass-fixture-runner>
          --tool=branch-counter
          --binary=$<TARGET_FILE:branch-counter>
          --inputs=${CMAKE_CURRENT_SOURCE_DIR}/inputs
          --golden=${CMAKE_CURRENT_SOURCE_DIR}/golden
)

add_test(
  NAME branch_counter_missing_input
  COMMAND sh -c "$<TARGET_FILE:branch-counter> /no/such/file.bc; test $? -eq 1"
)

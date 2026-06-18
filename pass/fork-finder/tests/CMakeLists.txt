add_test(
  NAME fork_finder_version
  COMMAND $<TARGET_FILE:fork-finder> --version
)
set_tests_properties(fork_finder_version PROPERTIES
  PASS_REGULAR_EXPRESSION "fork-finder 0\\."
)

add_executable(test_types unit/test_types.cpp)
target_link_libraries(test_types PRIVATE ForkFinderCore)
add_test(NAME test_types COMMAND test_types)

add_executable(test_report unit/test_report.cpp)
target_link_libraries(test_report PRIVATE ForkFinderCore)
add_test(NAME test_report COMMAND test_report)

add_test(
  NAME fork_finder_fixtures
  COMMAND $<TARGET_FILE:pass-fixture-runner>
          --tool=fork-finder
          --binary=$<TARGET_FILE:fork-finder>
          --inputs=${CMAKE_CURRENT_SOURCE_DIR}/inputs
          --golden=${CMAKE_CURRENT_SOURCE_DIR}/golden
)

add_executable(test_pretty unit/test_pretty.cpp)
target_link_libraries(test_pretty PRIVATE ForkFinderCore)
add_test(NAME test_pretty COMMAND test_pretty)

add_executable(test_categories unit/test_categories.cpp)
target_link_libraries(test_categories PRIVATE ForkFinderCore)
add_test(NAME test_categories COMMAND test_categories)

add_test(
  NAME fork_finder_missing_input
  COMMAND sh -c "$<TARGET_FILE:fork-finder> /no/such/file.bc; test $? -eq 1"
)

#pragma once

#include <cstdlib>
#include <iostream>

// NDEBUG-safe replacement for <cassert>. Always evaluates the condition;
// on failure prints a diagnostic and exits with status 1 so CTest treats
// the test as failed instead of silently passing under -DNDEBUG.
#define SYMBOLON_CHECK(cond)                                                   \
  do {                                                                         \
    if (!(cond)) {                                                             \
      std::cerr << "CHECK failed: " #cond " at " << __FILE__ << ":"            \
                << __LINE__ << std::endl;                                      \
      std::exit(1);                                                            \
    }                                                                          \
  } while (0)

#include "utils/Assert.hpp"

#include <exception>
#include <iostream>

namespace utils::detail::assert {
void HandleFailedAssert(const char *filename, int lineNumber, const char *expresssion, const char *message) {
  std::cerr << "Assertion '" << expresssion << "' failed in " << filename << " on line " << lineNumber
            << " with message " << message << std::endl;
  std::terminate();
}
} // namespace  utils::detail::assert
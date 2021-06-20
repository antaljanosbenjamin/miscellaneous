#include "MinAbsSumOfTwo.hpp"

#include <algorithm>

namespace MinAbsSumOfTwo {

int solution(std::vector<int> &A) {
  std::sort(A.begin(), A.end(), [](const int lhs, const int rhs) {
    auto absLhs = std::abs(lhs);
    auto absRhs = std::abs(rhs);
    return (absLhs < absRhs) || ((absLhs == absRhs) && (lhs < rhs));
  });
  A.erase(unique(A.begin(), A.end()), A.end());
  auto minAbs = std::abs(A[0] * 2);
  const auto nUsefulIndices = A.size() - 1;
  for (auto index = 0U; index < nUsefulIndices; ++index) {
    minAbs = std::min(std::abs(2 * A[index]), std::min(std::abs(A[index] + A[index + 1]), minAbs));
    if (minAbs == 0) {
      return 0;
    }
  }
  return minAbs;
}

} // namespace MinAbsSumOfTwo

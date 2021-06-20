#include "MaxProfit.hpp"

#include <algorithm>

namespace MaxProfit {

int solution(std::vector<int> &A) {
  if (A.empty()) {
    return 0;
  }
  auto minValue = A[0];
  auto maxProfit = 0;
  for (const auto &a: A) {
    maxProfit = std::max(maxProfit, a - minValue);
    minValue = std::min(minValue, a);
  }
  return maxProfit;
}

} // namespace MaxProfit

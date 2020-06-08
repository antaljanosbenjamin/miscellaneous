#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

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

int main() {
  {
    std::vector<int> A{23171, 21011, 21123, 21366, 21013, 21367}; // NOLINT(readability-magic-numbers)
    std::cout << "356 == " << solution(A) << '\n';
  }
  return 0;
}
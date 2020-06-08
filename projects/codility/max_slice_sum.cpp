#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int solution(std::vector<int> &A) {
  int maxEnding = A[0];
  int maxSlice = A[0];
  for (auto index = 1U; index < A.size(); ++index) {
    maxEnding = std::max(maxEnding, 0) + A[index];
    maxSlice = std::max(maxSlice, maxEnding);
  }
  return maxSlice;
}

int main() {
  {
    std::vector<int> A{3, 2, -6, 4, 0}; // NOLINT(readability-magic-numbers)
    std::cout << "5 == " << solution(A) << '\n';
  }
  return 0;
}
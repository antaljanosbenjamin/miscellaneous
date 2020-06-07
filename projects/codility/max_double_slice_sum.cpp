#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int solution(std::vector<int> &A) {
  std::vector<int> endingHere(A.size(), 0);
  std::vector<int> startingHere(A.size(), 0);

  for (auto index = 1U; index < A.size(); ++index) {
    endingHere[index] = std::max(0, endingHere[index - 1] + A[index]);
  }

  for (auto index = A.size() - 2; index > 0; --index) {
    startingHere[index] = std::max(0, startingHere[index + 1] + A[index]);
  }

  auto maxDoubleSlice = 0;
  for (auto index = 1U; index < A.size() - 1; ++index) {
    maxDoubleSlice = std::max(maxDoubleSlice, endingHere[index - 1] + startingHere[index + 1]);
  }

  return maxDoubleSlice;
}

int main() {
  {
    std::vector<int> A{3, 2, 6, -1, 4, 5, -1, 2}; // NOLINT(readability-magic-numbers)
    std::cout << "17 == " << solution(A) << '\n';
  }
  {
    std::vector<int> A{3, 2, 6, -1, 4, 5, -1, 2}; // NOLINT(readability-magic-numbers)
    std::cout << "17 == " << solution(A) << '\n';
  }
  return 0;
}
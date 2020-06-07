
#include <algorithm>
#include <iostream>
#include <vector>

int solution(std::vector<int> &A) {
  const auto prefixSums = [&A]() {
    std::vector<int> prefixSums(A.size() + 1, 0U);
    for (auto index = 0U; index < A.size(); ++index) {
      prefixSums[index + 1] = A[index] + prefixSums[index];
    }
    return prefixSums;
  }();
  const auto getAverage = [&prefixSums](const int from, const int to) {
    return (prefixSums[to + 1] - prefixSums[from]) / static_cast<double>(to - from + 1);
  };

  auto minAverage = getAverage(0, 1);
  auto minAverageStartIndex = 0;
  // Slices at most 2 or 3 length have the global minimum average.
  // https://codesays.com/2014/solution-to-min-avg-two-slice-by-codility/
  for (auto i = 0U; i < A.size(); ++i) {
    for (auto j = i + 1; j < std::min(A.size(), static_cast<size_t>(i + 3)); ++j) {
      const auto average = getAverage(i, j);
      if (average < minAverage) {
        minAverage = average;
        minAverageStartIndex = i;
      }
    }
  }
  return minAverageStartIndex;
}

int main() {
  {
    std::vector<int> A{4, 2, 2, 5, 1, 5, 8}; // NOLINT(readability-magic-numbers)
    std::cout << "1 == " << solution(A) << '\n';
  }
  return 0;
}
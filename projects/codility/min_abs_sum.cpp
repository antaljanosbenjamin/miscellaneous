#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// https://codility.com/media/train/solution-min-abs-sum.pdf
int solution(std::vector<int> &A) {
  constexpr static int theoreticalMaxValue = 100;
  std::for_each(A.begin(), A.end(), [](int &a) { a = std::abs(a); });
  std::vector<int> nOccurrence(theoreticalMaxValue + 1, 0);
  auto maxValue = 0;
  auto maxSum = 0;
  for (const auto &a: A) {
    nOccurrence[a]++;
    maxSum += a;
    maxValue = std::max(maxValue, a);
  }
  std::vector<int> dp(maxSum + 1, -1);
  dp[0] = 0;
  for (auto a = 0; a <= maxValue; ++a) {
    if (nOccurrence[a] > 0) {
      for (auto sum = 0; sum < static_cast<int>(dp.size()); ++sum) {
        if (dp[sum] >= 0) {
          dp[sum] = nOccurrence[a];
        } else if (a <= sum) {
          dp[sum] = std::max(-1, dp[sum - a] - 1);
        }
      }
    }
  }
  auto result = maxSum;
  for (auto sum = 0; sum <= maxSum / 2; ++sum) {
    if (dp[sum] >= 0) {
      result = std::min(result, maxSum - 2 * sum);
    }
  }
  return result;
}

int main() {
  {
    std::vector<int> A{1, 5, 2, -2}; // NOLINT(readability-magic-numbers)
    std::cout << "0 == " << solution(A) << '\n';
  }
  return 0;
}
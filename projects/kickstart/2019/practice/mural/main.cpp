#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>

int main() {
  size_t T;
  std::cin >> T;
  for (auto testCase{1u}; testCase <= T; ++testCase) {
    size_t N;
    std::cin >> N;
    std::string beautyScores;
    std::cin >> beautyScores;
    std::transform(beautyScores.begin(), beautyScores.end(), beautyScores.begin(),
                   [](const auto &score) { return score - '0'; });
    const size_t windowSize = (N + 1) / 2;
    size_t numberOfWindows = N - windowSize + 1;
    auto sum = std::accumulate(beautyScores.begin(), beautyScores.begin() + windowSize, 0u);
    auto max = sum;
    for (auto i{0u}; i < numberOfWindows; ++i) {
      sum -= beautyScores[i];
      sum += beautyScores[i + windowSize];
      max = std::max(max, sum);
    }
    std::cout << "Case #" << testCase << ": " << max << std::endl;
  }
}

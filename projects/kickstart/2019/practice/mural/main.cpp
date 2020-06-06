#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>

int main() {
  size_t T{0};
  std::cin >> T;
  for (auto testCase{1U}; testCase <= T; ++testCase) {
    size_t N{0};
    std::cin >> N;
    std::string beautyScores;
    std::cin >> beautyScores;
    std::transform(beautyScores.begin(), beautyScores.end(), beautyScores.begin(),
                   [](const auto &score) { return static_cast<char>(score - '0'); });
    const size_t windowSize = (N + 1) / 2;
    size_t numberOfWindows = N - windowSize + 1;
    auto sum = std::accumulate(beautyScores.begin(), beautyScores.begin() + windowSize, 0U);
    auto max = sum;
    for (auto i{0U}; i < numberOfWindows; ++i) {
      sum -= beautyScores[i];
      sum += beautyScores[i + windowSize];
      max = std::max(max, sum);
    }
    std::cout << "Case #" << testCase << ": " << max << std::endl;
  }
}

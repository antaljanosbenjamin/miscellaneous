
#include <iostream>

int solution(int N) {
  auto n = static_cast<size_t>(N);
  auto hasOne = false;
  auto zeroCounter = 0U;
  auto maxGapSize = 0U;
  while (n > 0) {
    const auto isOne = n % 2 == 1;
    n = n >> 1;
    if (isOne) {
      hasOne = true;
      maxGapSize = std::max(zeroCounter, maxGapSize);
      zeroCounter = 0U;
    } else if (hasOne) {
      zeroCounter++;
    }
  }
  return maxGapSize;
}

int main() {
  std::cout << "2 == " << solution(9) << '\n';
  std::cout << "4 == " << solution(529) << '\n';
  std::cout << "1 == " << solution(20) << '\n';
  std::cout << "0 == " << solution(15) << '\n';
  std::cout << "0 == " << solution(32) << '\n';
  return 0;
}
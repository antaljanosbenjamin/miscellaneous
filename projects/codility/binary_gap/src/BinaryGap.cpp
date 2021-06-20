#include "BinaryGap.hpp"

#include <algorithm>

namespace BinaryGap {

int solution(int N) {
  auto n = static_cast<size_t>(N);
  auto hasOne = false;
  auto zeroCounter = 0U;
  auto maxGapSize = 0U;
  while (n > 0) {
    const auto isOne = n % 2 == 1;
    n = n >> 1U;
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

} // namespace BinaryGap

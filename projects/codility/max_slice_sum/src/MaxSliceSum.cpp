#include <algorithm>

#include "MaxSliceSum.hpp"

namespace MaxSliceSum {

int solution(std::vector<int> &A) {
  int maxEnding = A[0];
  int maxSlice = A[0];
  for (auto index = 1U; index < A.size(); ++index) {
    maxEnding = std::max(maxEnding, 0) + A[index];
    maxSlice = std::max(maxSlice, maxEnding);
  }
  return maxSlice;
}

} // namespace MaxSliceSum

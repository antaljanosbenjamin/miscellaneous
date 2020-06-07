
#include <algorithm>
#include <iostream>
#include <vector>

int solution(std::vector<int> &A) {
  std::sort(A.begin(), A.end(), [](const int lhs, const int rhs) {
    auto absLhs = std::abs(lhs);
    auto absRhs = std::abs(rhs);
    return (absLhs < absRhs) || ((absLhs == absRhs) && (lhs < rhs));
  });
  A.erase(unique(A.begin(), A.end()), A.end());
  auto minAbs = std::abs(A[0] * 2);
  const auto nUsefulIndices = A.size() - 1;
  for (auto index = 0U; index < nUsefulIndices; ++index) {
    minAbs = std::min(std::abs(2 * A[index]), std::min(std::abs(A[index] + A[index + 1]), minAbs));
    if (minAbs == 0) {
      return 0;
    }
  }
  return minAbs;
}

int main() {
  {
    std::vector<int> A{1, 4, -3};
    std::cout << "1 == " << solution(A) << '\n';
  }
  {
    std::vector<int> A{-8, 4, 5, -10, 3};
    std::cout << "3 == " << solution(A) << '\n';
  }
  {
    std::vector<int> A{-8, 4, 5, -4, 3};
    std::cout << "0 == " << solution(A) << '\n';
  }
  {
    std::vector<int> A{-8, 4, 4, 4, 3};
    std::cout << "4 == " << solution(A) << '\n';
  }
  {
    std::vector<int> A{1};
    std::cout << "2 == " << solution(A) << '\n';
  }
  {
    std::vector<int> A{1, 2};
    std::cout << "2 == " << solution(A) << '\n';
  }
  return 0;
}
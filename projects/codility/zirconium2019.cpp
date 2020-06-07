#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

struct FrontendAdvantage {
  int developerId{0};
  int advantage{0};
};
// cppcheck-suppress constParameter
int solution(std::vector<int> &A, std::vector<int> &B, int F) {
  std::vector<FrontendAdvantage> advantages;
  advantages.resize(A.size());
  for (auto devId = 0U; devId < A.size(); ++devId) {
    advantages[devId].developerId = static_cast<int>(devId);
    advantages[devId].advantage = A[devId] - B[devId];
  }
  std::sort(advantages.begin(), advantages.end(),
            [](const FrontendAdvantage &lhs, const FrontendAdvantage &rhs) { return lhs.advantage > rhs.advantage; });

  auto sum{0};

  for (auto advantageIndex = 0U; advantageIndex < A.size(); ++advantageIndex) {
    const auto advatage = advantages[advantageIndex];
    if (advantageIndex < static_cast<size_t>(F)) {
      sum += A[advatage.developerId];
    } else {
      sum += B[advatage.developerId];
    }
  }
  return sum;
}

int main() {
  {
    std::vector<int> A{4, 2, 1};                        // NOLINT(readability-magic-numbers)
    std::vector<int> B{2, 5, 3};                        // NOLINT(readability-magic-numbers)
    std::cout << "10 == " << solution(A, B, 2) << '\n'; // NOLINT(readability-magic-numbers)
  }
  {
    std::vector<int> A{7, 1, 4, 4};                     // NOLINT(readability-magic-numbers)
    std::vector<int> B{5, 3, 4, 3};                     // NOLINT(readability-magic-numbers)
    std::cout << "18 == " << solution(A, B, 2) << '\n'; // NOLINT(readability-magic-numbers)
  }
  {
    std::vector<int> A{5, 5, 5};                        // NOLINT(readability-magic-numbers)
    std::vector<int> B{5, 5, 5};                        // NOLINT(readability-magic-numbers)
    std::cout << "15 == " << solution(A, B, 1) << '\n'; // NOLINT(readability-magic-numbers)
  }
  return 0;
}
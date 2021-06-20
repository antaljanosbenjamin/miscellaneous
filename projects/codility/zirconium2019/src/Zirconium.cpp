#include "Zirconium.hpp"

#include <algorithm>
#include <unordered_set>

namespace Zirconium {

struct FrontendAdvantage {
  int developerId{0};
  int advantage{0};
};

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

} // namespace Zirconium

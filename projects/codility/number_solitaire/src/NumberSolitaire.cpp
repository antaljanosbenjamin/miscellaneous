#include "NumberSolitaire.hpp"

#include <algorithm>

namespace NumberSolitaire {

int solution(std::vector<int> &A) {
  const auto &values = A;
  std::vector<int> table(values.size(), values[0]);

  for (auto index = 1U; index < values.size(); ++index) {
    const auto maxDiceRoll = std::min(index, 6U);
    auto localMaxValue = table[index - 1] + values[index];
    for (auto diceRoll = 2U; diceRoll <= maxDiceRoll; ++diceRoll) {
      localMaxValue = std::max(localMaxValue, table[index - diceRoll] + values[index]);
    }
    table[index] = localMaxValue;
  }
  return *table.rbegin();
}

} // namespace NumberSolitaire

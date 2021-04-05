#include <iostream>
#include <vector>
#include "Minesweeper.hpp"

using Minesweeper = minesweeper::Minesweeper;
using GameLevel = minesweeper::GameLevel;
template <typename T>
using Result = minesweeper::Result<T>;
using OpenedField = minesweeper::OpenedField;

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
  auto minesweeper = Minesweeper::create(GameLevel::Beginner).value();

  const auto dimension = minesweeper.getSize().value();

  std::cout << "Created " << dimension.width << " x " << dimension.height << " sized game\n";

  const auto openInfo = minesweeper.open(0U, 0U).value();
  std::cout << "Number of opened fields is " << openInfo.newlyOpenedFields.size() << '\n';

  return 0;
}
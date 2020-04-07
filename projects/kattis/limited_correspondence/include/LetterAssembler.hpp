#pragma once

#include <iostream>
#include <string>
#include <vector>

class LetterAssembler {
public:
  struct AssemblerState {
    std::vector<bool> used;
    std::string firstLetter;
    std::string secondLetter;
    std::string best;
  };

  bool read(std::istream &inputStream);

  std::string assemble();

protected:
  void recursiveSearch(AssemblerState &state);

  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  std::vector<std::pair<std::string, std::string>> wordPairs{};
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  size_t maximumPossibleLength{0U};
};

void AddResult(LetterAssembler::AssemblerState &state);
void Truncate(std::string &strToTruncate, size_t amount);

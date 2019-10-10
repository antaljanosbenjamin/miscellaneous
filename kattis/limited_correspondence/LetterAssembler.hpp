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

  std::vector<std::pair<std::string, std::string>> wordPairs;
  size_t maximumPossibleLength;
};

void AddResult(LetterAssembler::AssemblerState &state);
void Truncate(std::string &strToTruncate, size_t amount);

// https://open.kattis.com/submissions/4374599

#include <iostream>

#include "LetterAssembler.hpp"

int main() {
  auto caseNumber = 1U;

  while (std::cin.good()) {
    LetterAssembler letterAssembler;
    if (letterAssembler.read(std::cin)) {
      std::cout << "Case " << caseNumber++ << ": " << letterAssembler.assemble() << std::endl;
    }
  }
  return 0;
}

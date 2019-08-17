// https://open.kattis.com/submissions/4374599

#include "LetterAssembler.hpp"
#include <fstream>
#include <iostream>

int main() {
  auto caseNumber = 1u;

  while (std::cin.good()) {
    LetterAssembler letterAssembler;
    letterAssembler.read(std::cin);
    std::cout << "Case " << caseNumber++ << ": " << letterAssembler.assemble() << std::endl;
  }
  return 0;
}

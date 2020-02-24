#include <array>
#include <sstream>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "LetterAssembler.hpp"

TEST_CASE("Empty stream") {
  LetterAssembler assembler;
  std::stringstream ss;
  REQUIRE_FALSE(assembler.read(ss));
}

TEST_CASE("Example input") {
  auto input = R"(5
are yo
you u
how nhoware
alan arala
dear de
8
i ie
ing ding
resp orres
ond pon
oyc y
hello hi
enj njo
or c
3
efgh efgh
d cd
abc ab
3
a ab
b bb
c cc)";
  std::stringstream ss;
  ss << input;
  auto results = std::array<std::string, 4>{"dearalanhowareyou", "ienjoycorresponding", "abcd", "IMPOSSIBLE"};
  LetterAssembler assembler;

  for (const auto &expectedResult: results) {
    REQUIRE(assembler.read(ss));
    CHECK(assembler.assemble() == expectedResult);
  }
  REQUIRE_FALSE(assembler.read(ss));
}
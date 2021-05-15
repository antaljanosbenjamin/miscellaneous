#include "LetterAssembler.hpp"

#include <algorithm>
#include <limits>
#include <set>
#include <tuple>


bool LetterAssembler::read(std::istream &inputStream) {
  auto pairCount = 0U;
  auto sumFirstWordLength = 0ULL;
  auto sumSecondWordLength = 0ULL;
  this->wordPairs.clear();

  inputStream >> pairCount;

  if (pairCount == 0) {
    return false;
  }

  this->wordPairs.reserve(pairCount);

  for (size_t pos = 0; pos < pairCount; ++pos) {
    std::string firstWord;
    std::string secondWord;

    inputStream >> firstWord >> secondWord;

    sumFirstWordLength += firstWord.length();
    sumSecondWordLength += secondWord.length();

    this->wordPairs.push_back(std::make_pair(std::move(firstWord), std::move(secondWord)));
  }

  inputStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  std::sort(this->wordPairs.begin(), wordPairs.end());
  this->maximumPossibleLength = std::min(sumFirstWordLength, sumSecondWordLength);

  return true;
}

std::string LetterAssembler::assemble() {
  AssemblerState state;
  state.firstLetter.reserve(maximumPossibleLength);
  state.secondLetter.reserve(maximumPossibleLength);
  state.used = std::vector<bool>(this->wordPairs.size(), false);
  this->recursiveSearch(state);
  if (state.best.empty()) {
    return "IMPOSSIBLE";
  }
  return state.best;
}

void LetterAssembler::recursiveSearch(AssemblerState &state) {
  auto actualPos = std::min(state.firstLetter.length(), state.secondLetter.length());
  for (auto i = 0U; i < this->wordPairs.size(); ++i) {
    if (!state.used[i]) {
      state.used[i] = true;
      const auto &firstWord = this->wordPairs[i].first;
      const auto &secondWord = this->wordPairs[i].second;
      state.firstLetter.append(firstWord);
      state.secondLetter.append(secondWord);
      auto endPos = std::min(state.firstLetter.length(), state.secondLetter.length());
      auto isGoodContinuation =
          std::equal(&state.firstLetter[actualPos], &state.firstLetter[endPos], &state.secondLetter[actualPos]);
      if (isGoodContinuation) {
        if (state.firstLetter.length() == state.secondLetter.length()) {
          AddResult(state);
        }
        recursiveSearch(state);
      }
      Truncate(state.firstLetter, this->wordPairs[i].first.length());
      Truncate(state.secondLetter, this->wordPairs[i].second.length());
      state.used[i] = false;
    }
  }
}

void AddResult(LetterAssembler::AssemblerState &state) {
  const auto &result = state.firstLetter;
  if (state.best.length() == 0 || result.length() < state.best.length() ||
      (result.length() == state.best.length() && result < state.best)) {
    state.best = result;
  }
}

void Truncate(std::string &strToTruncate, size_t amount) {
  strToTruncate.resize(strToTruncate.length() - amount);
}

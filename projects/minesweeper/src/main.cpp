#include <iostream>
#include <vector>

#include "Minesweeper.hpp"

int main() {
  CErrorInfo errorInfo;
  std::vector<char> errorMessage(100U, 0);
  errorInfo.errorMessage = errorMessage.data();
  GameHandle gameHandle{};
  minesweeper_new_game(&gameHandle, GameLevel::Expert, &errorInfo);
  std::cout << "Error message: \"" << std::string(errorInfo.errorMessage)
            << "\", error code: " << static_cast<uint32_t>(errorInfo.errorCode) << '\n';

  FieldFlagResult result;

  COpenInfo openInfo;
  std::vector<CFieldInfo> fieldInfos(100U, CFieldInfo{});
  openInfo.fieldInfos = fieldInfos.data();
  openInfo.fieldInfosLength = 0U;
  openInfo.fieldInfosMaxLength = fieldInfos.size();

  minesweeper_game_open(gameHandle, 0U, 0U, &openInfo, &errorInfo);
  std::cout << "Error message: \"" << std::string(errorInfo.errorMessage)
            << "\", error code: " << static_cast<uint32_t>(errorInfo.errorCode) << '\n';
  std::cout << openInfo.fieldInfosLength << '\n';

  return openInfo.fieldInfosLength;
  minesweeper_destroy_game(gameHandle);
}
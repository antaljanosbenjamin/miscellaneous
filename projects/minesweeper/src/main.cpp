#include <iostream>
#include <vector>

#include "Minesweeper.hpp"

int main() {
  CErrorInfo errorInfo{};
  constexpr auto maxErrorMessageLength = 100U;
  std::vector<char> errorMessage(maxErrorMessageLength, 0);
  errorInfo.errorMessage = errorMessage.data();
  errorInfo.errorMessageMaxLength = errorMessage.size();
  GameHandle gameHandle{};
  minesweeper_new_game(&gameHandle, GameLevel::Expert, &errorInfo);
  std::cout << "Error message: \"" << std::string(errorInfo.errorMessage)
            << "\", error code: " << static_cast<uint32_t>(errorInfo.errorCode) << '\n';

  COpenInfo openInfo{};
  constexpr auto fieldInfosLength = 100U;
  std::vector<CFieldInfo> fieldInfos(fieldInfosLength, CFieldInfo{});
  openInfo.fieldInfos = fieldInfos.data();
  openInfo.fieldInfosLength = 0U;
  openInfo.fieldInfosMaxLength = fieldInfos.size();

  minesweeper_game_open(gameHandle, 0U, 0U, &openInfo, &errorInfo);
  std::cout << "Error message: \"" << std::string(errorInfo.errorMessage)
            << "\", error code: " << static_cast<uint32_t>(errorInfo.errorCode) << '\n';
  std::cout << openInfo.fieldInfosLength << '\n';

  minesweeper_destroy_game(gameHandle);

  return 0;
}
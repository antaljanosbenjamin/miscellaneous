#include <iostream>
#include <vector>
#include "CMinesweeper.hpp"

int main() {
  CMErrorInfo errorInfo{};
  constexpr auto maxErrorMessageLength = 100U;
  std::vector<char> errorMessage(maxErrorMessageLength, 0);
  errorInfo.errorMessage = errorMessage.data();
  errorInfo.errorMessageMaxLength = errorMessage.size();
  CMGameHandle gameHandle{};
  minesweeper_new_game(&gameHandle, CMGameLevel::CMGL_Expert, &errorInfo);
  std::cout << "Error message: \"" << std::string(errorInfo.errorMessage)
            << "\", error code: " << static_cast<uint32_t>(errorInfo.errorCode) << '\n';
  if (errorInfo.errorCode != CMError::CME_Ok) {
    return -1;
  }

  CMOpenInfo openInfo{};
  constexpr auto fieldInfosLength = 100U;
  std::vector<CMFieldInfo> fieldInfos(fieldInfosLength, CMFieldInfo{});
  openInfo.fieldInfos = fieldInfos.data();
  openInfo.fieldInfosLength = 0U;
  openInfo.fieldInfosMaxLength = fieldInfos.size();

  minesweeper_game_open(gameHandle, 0U, 0U, &openInfo, &errorInfo);
  std::cout << "Error message: \"" << std::string(errorInfo.errorMessage)
            << "\", error code: " << static_cast<uint32_t>(errorInfo.errorCode) << '\n';
  if (errorInfo.errorCode != CMError::CME_Ok) {
    return -2;
  }
  std::cout << openInfo.fieldInfosLength << '\n';

  minesweeper_destroy_game(&gameHandle, &errorInfo);

  return 0;
}
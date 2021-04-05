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
  constexpr auto newlyOpenedFieldsLength = 100U;
  std::vector<CMOpenedField> newlyOpenedFields(newlyOpenedFieldsLength, CMOpenedField{});
  openInfo.newlyOpenedFields = newlyOpenedFields.data();
  openInfo.newlyOpenedFieldsLength = 0U;
  openInfo.newlyOpenedFieldsMaxLength = newlyOpenedFields.size();

  minesweeper_game_open(gameHandle, 0U, 0U, &openInfo, &errorInfo);
  std::cout << "Error message: \"" << std::string(errorInfo.errorMessage)
            << "\", error code: " << static_cast<uint32_t>(errorInfo.errorCode) << '\n';
  if (errorInfo.errorCode != CMError::CME_Ok) {
    return -2;
  }
  std::cout << openInfo.newlyOpenedFieldsLength << '\n';

  minesweeper_destroy_game(&gameHandle, &errorInfo);

  return 0;
}
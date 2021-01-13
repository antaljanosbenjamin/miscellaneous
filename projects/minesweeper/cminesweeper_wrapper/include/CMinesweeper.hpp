#pragma once

#include <cstdint>

extern "C" {

enum CMError : uint32_t {
  CME_Ok,
  CME_InvalidInput,
  CME_NullPointerAsInput,
  CME_IndexIsOutOfRange,
  CME_InsufficientBuffer,
  CME_UnexpectedError,
};

enum CMFieldTypeTag : uint32_t {
  CMFTT_Empty,
  CMFTT_Numbered,
  CMFTT_Mine,
};

struct CMFieldTagNumberedPayload {
  uint8_t value;
};

struct CMFieldType {
  CMFieldTypeTag tag{CMFieldTypeTag::CMFTT_Empty};
  CMFieldTagNumberedPayload payload{0U};
};

struct CMFieldInfo {
  uint64_t row{0U};
  uint64_t col{0U};
  CMFieldType fieldType{};
};

enum CMOpenResult {
  CMOR_Ok,
  CMOR_IsFlagged,
  CMOR_Boom,
  CMOR_Winner,
};

struct CMOpenInfo {
  CMOpenResult result{CMOR_Ok};
  uint64_t fieldInfosLength{0U};
  uint64_t fieldInfosMaxLength{0U};
  CMFieldInfo *fieldInfos{nullptr};
};

struct CMErrorInfo {
  CMError errorCode{CMError::CME_UnexpectedError};
  uint64_t errorMessageLength{0U};
  uint64_t errorMessageMaxLength{0U};
  char *errorMessage{nullptr};
};

enum CMGameLevel {
  CMGL_Beginner,
  CMGL_Intermediate,
  CMGL_Expert,
};

enum CMFieldFlagResult {
  CMFFR_Flagged,
  CMFFR_FlagRemoved,
  CMFFR_AlreadyOpened,
};

using CMGameHandle = struct CMGame *;
using ConstCMGameHandle = const CMGame *;

void minesweeper_new_game(CMGameHandle *gameHandle, CMGameLevel level, CMErrorInfo *errorInfo);
void minesweeper_game_open(CMGameHandle gameHandle, const uint64_t row, const uint64_t column, CMOpenInfo *openInfo,
                           CMErrorInfo *errorInfo);
void minesweeper_game_toggle_flag(CMGameHandle gameHandle, const uint64_t row, const uint64_t column,
                                  CMFieldFlagResult *flagResult, CMErrorInfo *errorInfo);
void minesweeper_game_get_width(ConstCMGameHandle gameHandle, uint64_t *width, CMErrorInfo *errorInfo);
void minesweeper_game_get_height(ConstCMGameHandle gameHandle, uint64_t *height, CMErrorInfo *errorInfo);
void minesweeper_game_get_elapsed_seconds(ConstCMGameHandle gameHandle, uint64_t *elapsed_seconds,
                                          CMErrorInfo *errorInfo);
void minesweeper_destroy_game(CMGameHandle *gameHandle, CMErrorInfo *errorInfo);
}
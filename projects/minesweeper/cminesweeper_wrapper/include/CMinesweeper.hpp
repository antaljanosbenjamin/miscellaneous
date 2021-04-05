#pragma once

#include <cstdint>

extern "C" {

using CMGameSizeType = int64_t;
using CMArraySizeType = uint64_t;

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

struct CMOpenedField {
  CMGameSizeType row{0U};
  CMGameSizeType col{0U};
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
  CMArraySizeType newlyOpenedFieldsLength{0U};
  CMArraySizeType newlyOpenedFieldsMaxLength{0U};
  CMOpenedField *newlyOpenedFields{nullptr};
};

struct CMErrorInfo {
  CMError errorCode{CMError::CME_UnexpectedError};
  CMArraySizeType errorMessageLength{0U};
  CMArraySizeType errorMessageMaxLength{0U};
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
void minesweeper_game_open(CMGameHandle gameHandle, const CMGameSizeType row, const CMGameSizeType column,
                           CMOpenInfo *openInfo, CMErrorInfo *errorInfo);
void minesweeper_game_toggle_flag(CMGameHandle gameHandle, const CMGameSizeType row, const CMGameSizeType column,
                                  CMFieldFlagResult *flagResult, CMErrorInfo *errorInfo);
void minesweeper_game_get_width(ConstCMGameHandle gameHandle, CMGameSizeType *width, CMErrorInfo *errorInfo);
void minesweeper_game_get_height(ConstCMGameHandle gameHandle, CMGameSizeType *height, CMErrorInfo *errorInfo);
void minesweeper_game_get_elapsed_seconds(ConstCMGameHandle gameHandle, uint64_t *elapsed_seconds,
                                          CMErrorInfo *errorInfo);
void minesweeper_destroy_game(CMGameHandle *gameHandle, CMErrorInfo *errorInfo);
}
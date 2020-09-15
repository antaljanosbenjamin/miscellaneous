#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum CMError : uint32_t {
  CME_Ok,
  CME_InvalidInput,
  CME_NullPointerAsInput,
  CME_IndexIsOutOfRange,
  CME_InsufficientBuffer,
  CME_UnexpectedError,
} CMError;

typedef enum CMFieldTypeTag : uint32_t {
  CMFTT_Empty,
  CMFTT_Numbered,
  CMFTT_Mine,
} CMFieldTypeTag;

typedef struct CMFieldTagNumberedPayload {
  uint8_t value;
} CMFieldTagNumberedPayload;

typedef struct CMFieldType {
  CMFieldTypeTag tag;
  CMFieldTagNumberedPayload payload;
} CMFieldType;

typedef struct CMFieldInfo {
  uint64_t row;
  uint64_t col;
  CMFieldType fieldType;
} CMFieldInfo;

typedef enum CMOpenResult {
  CMOR_Ok,
  CMOR_IsFlagged,
  CMOR_Boom,
  CMOR_Winner,
} CMOpenResult;

typedef struct CMOpenInfo {
  CMOpenResult result;
  uint64_t fieldInfosLength;
  uint64_t fieldInfosMaxLength;
  CMFieldInfo *fieldInfos;
} CMOpenInfo;

struct CMErrorInfo {
  CMError errorCode;
  uint64_t errorMessageLength{0U};
  uint64_t errorMessageMaxLength{0U};
  char *errorMessage{nullptr};
};

typedef enum CMGameLevel {
  CMGL_Beginner,
  CMGL_Intermediate,
  CMGL_Expert,
} CMGameLevel;

typedef enum CMFieldFlagResult {
  CMFFR_Flagged,
  CMFFR_FlagRemoved,
  CMFFR_AlreadyOpened,
} CMFieldFlagResult;

typedef struct CMGame CMGame;
typedef CMGame *CMGameHandle;
typedef const CMGame *ConstCMGameHandle;

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

#ifdef __cplusplus
}
#endif
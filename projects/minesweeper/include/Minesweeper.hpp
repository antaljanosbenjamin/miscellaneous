#include <cstdint>

enum class CError : uint32_t {
  Ok,
  NullPointerAsInput,
  InvalidInput,
  InsufficientBuffer,
  UnexpectedError,
};

enum class FieldTypeTag : uint32_t {
  Empty,
  Numbered,
  Mine,
};

struct FieldTagNumberedPayload {
  uint8_t value;
};

struct FieldType {
  FieldTypeTag tag;
  FieldTagNumberedPayload payload;
};

struct CFieldInfo {
  uint64_t row;
  uint64_t col;
  FieldType fieldType;
};

enum class OpenResult {
  Ok,
  IsFlagged,
  Boom,
  Winner,
};

struct COpenInfo {
  OpenResult result;
  uint64_t fieldInfosLength;
  uint64_t fieldInfosMaxLength;
  CFieldInfo *fieldInfos;
};

struct CErrorInfo {
  CError errorCode;
  uint64_t errorMessageLength;
  uint64_t errorMessageMaxLendth;
  char *errorMessage;
};

enum class GameLevel {
  Beginner,
  Intermediate,
  Expert,
};

struct Game;
using GameHandle = Game *;

extern "C" {

void new_game(GameHandle *gameHandle, GameLevel level, CErrorInfo *errorInfo);
void game_open(const GameHandle gameHandle, const uint64_t row, const uint64_t column, COpenInfo *openInfo,
               CErrorInfo *errorInfo);
void destroy_game(const GameHandle gameHandle);
}
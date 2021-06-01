#pragma once

#include <memory>
#include <optional>
#include <string>
#include <tl/expected.hpp>
#include "PropagateConstWrapper.hpp"

namespace minesweeper {

enum class Error {
  Ok,
  InvalidInput,
  IndexIsOutOfRange,
  ConversionError,
  UnexpectedError,
};

enum class GameLevel {
  Beginner,
  Intermediate,
  Expert,
};

enum class OpenResult {
  Ok,
  IsFlagged,
  Boom,
  Winner,
};

enum class FieldFlagResult {
  Flagged,
  FlagRemoved,
  AlreadyOpened,
};

enum class FieldType : uint8_t {
  Empty = 0,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Mine,
};

using SizeType = int64_t;

#pragma pack(push, 1)
struct OpenedField {
  SizeType row{0};
  SizeType column{0};
  FieldType type{FieldType::Empty};
};
#pragma pack(pop)

struct OpenInfo {
  OpenResult openResult;
  std::vector<OpenedField> newlyOpenedFields;
};

template <typename TResult>
using Result = tl::expected<TResult, Error>;
constexpr auto in_place = tl::in_place;

class Minesweeper {
public:
  struct Dimension {
    SizeType width{0};
    SizeType height{0};
  };

  [[nodiscard]] static Result<Minesweeper> create(GameLevel level) noexcept;

  Minesweeper(const Minesweeper &) = delete;
  Minesweeper(Minesweeper &&other) noexcept;
  Minesweeper &operator=(const Minesweeper &) = delete;
  Minesweeper &operator=(Minesweeper &&other) noexcept;

  ~Minesweeper() noexcept;

  [[nodiscard]] Result<Dimension> getSize() const noexcept;
  [[nodiscard]] Result<uint64_t> getElapsedSeconds() const noexcept;
  [[nodiscard]] Result<FieldFlagResult> toggleFlag(const SizeType row, const SizeType column);
  [[nodiscard]] Result<OpenInfo> open(const SizeType row, const SizeType column);
  [[nodiscard]] Result<OpenInfo> openNeighbors(const SizeType row, const SizeType column);

private:
  struct Impl;

  explicit Minesweeper(std::unique_ptr<Impl> &&impl);

  std::experimental::propagate_const<std::unique_ptr<Impl>> impl;
};
} // namespace minesweeper

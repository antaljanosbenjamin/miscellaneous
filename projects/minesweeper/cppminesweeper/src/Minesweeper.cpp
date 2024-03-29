#include "Minesweeper.hpp"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "CMinesweeper.hpp"

static_assert(std::is_same_v<minesweeper::SizeType, CMGameSizeType>, "Type mistmatch!");

namespace {
minesweeper::Error convert(const CMError cError) {
  switch (cError) {
  case CMError::CME_Ok:
    return minesweeper::Error::Ok;
  case CMError::CME_IndexIsOutOfRange:
    return minesweeper::Error::IndexIsOutOfRange;
  case CMError::CME_InvalidInput:
  case CMError::CME_NullPointerAsInput:
  case CMError::CME_InsufficientBuffer:
  case CMError::CME_UnexpectedError:
    return minesweeper::Error::UnexpectedError;
  }
  return minesweeper::Error::UnexpectedError;
}

minesweeper::OpenedField convertOpenedField(const CMOpenedField &cOpenedField) {
  const auto getFieldType = [&cOpenedField]() {
    switch (cOpenedField.fieldType.tag) {
    case CMFieldTypeTag::CMFTT_Empty:
      return minesweeper::FieldType::Empty;
    case CMFieldTypeTag::CMFTT_Mine:
      return minesweeper::FieldType::Mine;
    case CMFieldTypeTag::CMFTT_Numbered:
      assert(cOpenedField.fieldType.payload.value <= 9U && "Field type payload cannot be greater than 9!");
      return minesweeper::FieldType{cOpenedField.fieldType.payload.value};
    }

    throw std::runtime_error(
        "Unexpected CMFieldTypeTag value: " +
        std::to_string(static_cast<std::underlying_type_t<CMFieldTypeTag>>(cOpenedField.fieldType.tag)));
  };

  return minesweeper::OpenedField{cOpenedField.row, cOpenedField.col, getFieldType()};
}

minesweeper::OpenResult convert(const CMOpenResult cOpenResult) {
  switch (cOpenResult) {
  case CMOpenResult::CMOR_Ok:
    return minesweeper::OpenResult::Ok;
  case CMOpenResult::CMOR_IsFlagged:
    return minesweeper::OpenResult::IsFlagged;
  case CMOpenResult::CMOR_Boom:
    return minesweeper::OpenResult::Boom;
  case CMOpenResult::CMOR_Winner:
    return minesweeper::OpenResult::Winner;
  }

  throw std::runtime_error("Unexpected CMOpenResult value: " +
                           std::to_string(static_cast<std::underlying_type_t<CMOpenResult>>(cOpenResult)));
}

template <typename TCppType>
struct CType {};

template <typename TCppType>
using CType_t = typename CType<TCppType>::type;

template <>
struct CType<minesweeper::FieldFlagResult> {
  using type = CMFieldFlagResult;
};

template <>
struct CType<uint64_t> {
  using type = uint64_t;
};

template <>
struct CType<minesweeper::SizeType> {
  using type = CMGameSizeType;
};

uint64_t convert(const uint64_t cValue) {
  return cValue;
}

minesweeper::FieldFlagResult convert(const CMFieldFlagResult cFieldFlagResult) {
  switch (cFieldFlagResult) {
  case CMFieldFlagResult::CMFFR_Flagged:
    return minesweeper::FieldFlagResult::Flagged;
  case CMFieldFlagResult::CMFFR_FlagRemoved:
    return minesweeper::FieldFlagResult::FlagRemoved;
  case CMFieldFlagResult::CMFFR_AlreadyOpened:
    return minesweeper::FieldFlagResult::AlreadyOpened;
  }

  throw std::runtime_error("Unexpected CMFieldFlagResult value: " +
                           std::to_string(static_cast<std::underlying_type_t<CMFieldFlagResult>>(cFieldFlagResult)));
}

CMGameLevel convert(const minesweeper::GameLevel gameLevel) {
  switch (gameLevel) {
  case minesweeper::GameLevel::Beginner:
    return CMGameLevel::CMGL_Beginner;
  case minesweeper::GameLevel::Intermediate:
    return CMGameLevel::CMGL_Intermediate;
  case minesweeper::GameLevel::Expert:
    return CMGameLevel::CMGL_Expert;
  }
  throw std::runtime_error("Unexpected GameLevel value: " +
                           std::to_string(static_cast<std::underlying_type_t<minesweeper::GameLevel>>(gameLevel)));
}

class ErrorInfo {
public:
  constexpr static size_t defaultErrorMessageMaxLength = 100;
  explicit ErrorInfo(size_t errorMessageMaxLength = defaultErrorMessageMaxLength)
    : buffer(errorMessageMaxLength, 0) {
    this->cErrorInfo.errorMessage = buffer.data();
    this->cErrorInfo.errorMessageMaxLength = buffer.size();
  }

  [[maybe_unused]] [[nodiscard]] bool isOk() const noexcept {
    return cErrorInfo.errorCode == CMError::CME_Ok;
  }

  [[nodiscard]] minesweeper::Error getCode() const noexcept {
    return convert(cErrorInfo.errorCode);
  }

  [[maybe_unused]] [[nodiscard]] std::string_view getErrorMessage() const noexcept {
    return std::string_view{buffer.data(), cErrorInfo.errorMessageLength};
  }

  template <typename TFunc, typename... TArgs>
  bool call(const TFunc &func, TArgs &&... args) noexcept {
    reset();
    func(std::forward<TArgs>(args)..., &cErrorInfo);
    return cErrorInfo.errorCode == CMError::CME_Ok;
  }

private:
  void reset() noexcept {
    cErrorInfo.errorCode = CMError::CME_Ok;
    cErrorInfo.errorMessageLength = 0;
  };

  CMErrorInfo cErrorInfo{CMError::CME_Ok, 0U, 0U, nullptr};
  std::vector<char> buffer;
};

template <typename TValue, typename TFunc, typename... TArgs>
// NOLINTNEXTLINE(misc-misplaced-const)
minesweeper::Result<TValue> getValue(const TFunc &func, const CMGameHandle handle, ErrorInfo &errorInfo,
                                     TArgs &&... args) noexcept {
  CType_t<TValue> cValue;
  if (errorInfo.call(func, handle, std::forward<TArgs>(args)..., &cValue)) {
    try {
      return convert(cValue);
    } catch (...) {
      return tl::unexpected{minesweeper::Error::ConversionError};
    }
  }
  return tl::unexpected{errorInfo.getCode()};
}

template <typename TValue, typename TFunc>
// NOLINTNEXTLINE(misc-misplaced-const)
minesweeper::Result<TValue> getValue(const TFunc &func, const CMGameHandle handle, ErrorInfo &errorInfo) noexcept {
  CType_t<TValue> cValue;
  if (errorInfo.call(func, handle, &cValue)) {
    try {
      return convert(cValue);
    } catch (...) {
      return tl::unexpected{minesweeper::Error::ConversionError};
    }
  }
  return tl::unexpected{errorInfo.getCode()};
}
} // namespace

namespace minesweeper {
struct Minesweeper::Impl {
  using OpenFunctionType = decltype(minesweeper_game_open);
  static_assert(std::is_same_v<OpenFunctionType, decltype(minesweeper_game_open_neighbors)>,
                "The two open function has different type!");

  [[nodiscard]] Result<Dimension> getSize() const noexcept {
    return getValue<minesweeper::SizeType>(minesweeper_game_get_width, this->handle, this->errorInfo)
        .and_then([this](const Result<minesweeper::SizeType> &width) {
          return getValue<minesweeper::SizeType>(minesweeper_game_get_height, this->handle, this->errorInfo)
              .and_then([width](const Result<minesweeper::SizeType> &height) -> Result<Minesweeper::Dimension> {
                return Dimension{*width, *height};
              });
        });
  }

  [[nodiscard]] Result<uint64_t> getElapsedSeconds() const noexcept {
    return getValue<uint64_t>(minesweeper_game_get_elapsed_seconds, this->handle, this->errorInfo);
  }

  Result<FieldFlagResult> toggleFlag(const uint64_t row, const uint64_t column) {
    return getValue<FieldFlagResult>(minesweeper_game_toggle_flag, this->handle, this->errorInfo, row, column);
  }

  [[nodiscard]] Result<OpenInfo> openCommon(const uint64_t row, const uint64_t column, OpenFunctionType &openFunc) {
    if (!this->openedFieldBuffer.has_value()) {
      auto buffer =
          this->getSize().and_then([](const Result<Dimension> &dimension) -> Result<std::vector<CMOpenedField>> {
            assert(dimension->height != 0U);
            assert(dimension->width != 0U);
            const auto bufferSize = static_cast<CMArraySizeType>(dimension->width * dimension->height);

            if (static_cast<minesweeper::SizeType>(bufferSize / dimension->width) != dimension->height) {
              return tl::unexpected{Error::UnexpectedError};
            }

            return std::vector<CMOpenedField>{bufferSize};
          });

      if (!buffer.has_value()) {
        return tl::unexpected{buffer.error()};
      }
      this->openedFieldBuffer = std::move(*buffer);
    }

    CMOpenInfo cOpenInfo{CMOpenResult::CMOR_Boom, 0U, this->openedFieldBuffer->size(), this->openedFieldBuffer->data()};

    if (!this->errorInfo.call(openFunc, this->handle, row, column, &cOpenInfo)) {
      return tl::unexpected{errorInfo.getCode()};
    }
    std::vector<OpenedField> newlyOpenedFields;
    std::transform(this->openedFieldBuffer->begin(),
                   this->openedFieldBuffer->begin() + cOpenInfo.newlyOpenedFieldsLength,
                   std::back_inserter(newlyOpenedFields), convertOpenedField);

    return OpenInfo{convert(cOpenInfo.result), std::move(newlyOpenedFields)};
  }

  [[nodiscard]] Result<OpenInfo> open(const uint64_t row, const uint64_t column) {
    return this->openCommon(row, column, minesweeper_game_open);
  }

  [[nodiscard]] Result<OpenInfo> openNeighbors(const uint64_t row, const uint64_t column) {
    return this->openCommon(row, column, minesweeper_game_open_neighbors);
  }

  CMGameHandle handle{nullptr};
  mutable ErrorInfo errorInfo{};
  std::optional<std::vector<CMOpenedField>> openedFieldBuffer{};
};

Minesweeper::Minesweeper(Minesweeper &&other) noexcept = default;

Minesweeper &Minesweeper::operator=(Minesweeper &&) noexcept = default;

Minesweeper::~Minesweeper() noexcept {
  if (nullptr != this->impl) {
    this->impl->errorInfo.call(minesweeper_destroy_game, &this->impl->handle);
  }
}

Result<Minesweeper> Minesweeper::create(const GameLevel level) noexcept {
  CMGameLevel cLevel{CMGameLevel::CMGL_Beginner};
  try {
    cLevel = convert(level);
  } catch (...) {
    return tl::unexpected{minesweeper::Error::ConversionError};
  }

  ErrorInfo errorInfo;
  CMGameHandle handle{nullptr};
  if (errorInfo.call(minesweeper_new_game, &handle, cLevel)) {
    Impl impl{};
    impl.handle = handle;
    return Minesweeper{std::make_unique<Impl>(std::move(impl))};
  };
  return tl::unexpected{errorInfo.getCode()};
} // namespace minesweeper

Result<Minesweeper::Dimension> Minesweeper::getSize() const noexcept {
  return this->impl->getSize();
}

Result<uint64_t> Minesweeper::getElapsedSeconds() const noexcept {
  return this->impl->getElapsedSeconds();
}

Result<FieldFlagResult> Minesweeper::toggleFlag(const minesweeper::SizeType row, const minesweeper::SizeType column) {
  return this->impl->toggleFlag(row, column);
}

[[nodiscard]] Result<OpenInfo> Minesweeper::open(const minesweeper::SizeType row, const minesweeper::SizeType column) {
  return this->impl->open(row, column);
}

[[nodiscard]] Result<OpenInfo> Minesweeper::openNeighbors(const minesweeper::SizeType row,
                                                          const minesweeper::SizeType column) {
  return this->impl->openNeighbors(row, column);
}

Minesweeper::Minesweeper(std::unique_ptr<Impl> &&impl)
  : impl(std::move(impl)) {
}
} // namespace minesweeper

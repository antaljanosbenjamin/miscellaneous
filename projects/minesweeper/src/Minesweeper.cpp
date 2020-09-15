#include "Minesweeper.hpp"

#include <cassert>
#include <utility>

#include "CMinesweeper.hpp"

namespace {
constexpr minesweeper::Error convert(const CMError cError) {
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
}

constexpr minesweeper::FieldInfo convertFieldInfo(const CMFieldInfo &cFieldInfo) {
  const auto getFieldType = [&cFieldInfo]() {
    switch (cFieldInfo.fieldType.tag) {
    case CMFieldTypeTag::CMFTT_Empty:
      return minesweeper::FieldType::Empty;
    case CMFieldTypeTag::CMFTT_Mine:
      return minesweeper::FieldType::Mine;
    case CMFieldTypeTag::CMFTT_Numbered:
      assert(cFieldInfo.fieldType.payload.value <= 9U && "Field type payload cannot be greater than 9!");
      return minesweeper::FieldType{cFieldInfo.fieldType.payload.value};
    }
  };

  minesweeper::FieldInfo fieldInfo{cFieldInfo.row, cFieldInfo.col, getFieldType()};

  return fieldInfo;
}

constexpr minesweeper::OpenResult convert(const CMOpenResult cOpenResult) {
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

constexpr uint64_t convert(const uint64_t cValue) {
  return cValue;
}

constexpr minesweeper::FieldFlagResult convert(const CMFieldFlagResult cFieldFlagResult) {
  switch (cFieldFlagResult) {
  case CMFieldFlagResult::CMFFR_Flagged:
    return minesweeper::FieldFlagResult::Flagged;
  case CMFieldFlagResult::CMFFR_FlagRemoved:
    return minesweeper::FieldFlagResult::FlagRemoved;
  case CMFieldFlagResult::CMFFR_AlreadyOpened:
    return minesweeper::FieldFlagResult::AlreadyOpened;
  }
}

constexpr CMGameLevel convert(const minesweeper::GameLevel gameLevel) {
  switch (gameLevel) {
  case minesweeper::GameLevel::Beginner:
    return CMGameLevel::CMGL_Beginner;
  case minesweeper::GameLevel::Intermediate:
    return CMGameLevel::CMGL_Intermediate;
  case minesweeper::GameLevel::Expert:
    return CMGameLevel::CMGL_Expert;
  }
}

class ErrorInfo {
public:
  explicit ErrorInfo(size_t errorMessageMaxLength = 100)
    : cErrorInfo{CMError::CME_Ok, 0U, 0U, nullptr}
    , buffer(errorMessageMaxLength, 0) {
    this->cErrorInfo.errorMessage = buffer.data();
    this->cErrorInfo.errorMessageMaxLength = buffer.size();
  }

  bool isOk() const noexcept {
    return cErrorInfo.errorCode == CMError::CME_Ok;
  }

  minesweeper::Error getCode() const noexcept {
    return convert(cErrorInfo.errorCode);
  }

  std::string_view getErrorMessage() const noexcept {
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

  CMErrorInfo cErrorInfo;
  std::vector<char> buffer;
};

template <typename TValue, typename TFunc, typename... TArgs>
minesweeper::Result<TValue> getValue(const TFunc &func, const CMGameHandle handle, ErrorInfo &errorInfo,
                                     TArgs &&... args) noexcept {
  CType_t<TValue> cValue;
  if (errorInfo.call(func, handle, std::forward<TArgs>(args)..., &cValue)) {
    return convert(cValue);
  }
  return tl::unexpected{errorInfo.getCode()};
}

template <typename TValue, typename TFunc>
minesweeper::Result<TValue> getValue(const TFunc &func, const CMGameHandle handle, ErrorInfo &errorInfo) noexcept {
  CType_t<TValue> cValue;
  if (errorInfo.call(func, handle, &cValue)) {
    return convert(cValue);
  }
  return tl::unexpected{errorInfo.getCode()};
}
} // namespace

namespace minesweeper {
struct Minesweeper::Impl {
  [[nodiscard]] Result<Dimension> getSize() const noexcept {
    return getValue<uint64_t>(minesweeper_game_get_width, this->handle, this->errorInfo)
        .and_then([this](const Result<uint64_t> &width) {
          return getValue<uint64_t>(minesweeper_game_get_height, this->handle, this->errorInfo)
              .and_then([width](const Result<uint64_t> &height) -> Result<Minesweeper::Dimension> {
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

  [[nodiscard]] Result<OpenInfo> open(const uint64_t row, const uint64_t column) {
    if (!this->fieldInfoBuffer.has_value()) {
      auto buffer =
          this->getSize().and_then([](const Result<Dimension> &dimension) -> Result<std::vector<CMFieldInfo>> {
            assert(dimension->height != 0U);
            assert(dimension->width != 0U);
            const auto bufferSize = dimension->width * dimension->height;

            if (bufferSize / dimension->width != dimension->height) {
              return tl::unexpected{Error::UnexpectedError};
            }

            return std::vector<CMFieldInfo>{bufferSize};
          });

      if (!buffer.has_value()) {
        return tl::unexpected{buffer.error()};
      }
      fieldInfoBuffer = std::move(*buffer);
    }

    CMOpenInfo cOpenInfo{CMOpenResult::CMOR_Boom, 0U, this->fieldInfoBuffer->size(), this->fieldInfoBuffer->data()};

    if (!this->errorInfo.call(minesweeper_game_open, this->handle, row, column, &cOpenInfo)) {
      return tl::unexpected{errorInfo.getCode()};
    }
    std::vector<FieldInfo> fieldInfos;
    std::transform(this->fieldInfoBuffer->begin(), this->fieldInfoBuffer->begin() + cOpenInfo.fieldInfosLength,
                   std::back_inserter(fieldInfos), convertFieldInfo);

    return OpenInfo{convert(cOpenInfo.result), std::move(fieldInfos)};
  }

  CMGameHandle handle{nullptr};
  mutable ErrorInfo errorInfo{};
  std::optional<std::vector<CMFieldInfo>> fieldInfoBuffer{};
};

Minesweeper::Minesweeper(Minesweeper &&other) noexcept = default;

Minesweeper &Minesweeper::operator=(Minesweeper &&) noexcept = default;

Minesweeper::~Minesweeper() noexcept {
  if (nullptr != this->impl) {
    this->impl->errorInfo.call(minesweeper_destroy_game, &this->impl->handle);
  }
}

Result<Minesweeper> Minesweeper::create(const GameLevel level) noexcept {
  ErrorInfo errorInfo;
  CMGameHandle handle{nullptr};
  if (errorInfo.call(minesweeper_new_game, &handle, convert(level))) {
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

Result<FieldFlagResult> Minesweeper::toggleFlag(const uint64_t row, const uint64_t column) {
  return this->impl->toggleFlag(row, column);
}

[[nodiscard]] Result<OpenInfo> Minesweeper::open(const uint64_t row, const uint64_t column) {
  return this->impl->open(row, column);
}

Minesweeper::Minesweeper(std::unique_ptr<Impl> &&impl)
  : impl(std::move(impl)) {
}
} // namespace minesweeper

#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>

#include "utils/Assert.hpp"

namespace utils {

class DisjointSets {
public:
  using ValueType = int64_t;

  [[nodiscard]] int64_t size() const noexcept;

  bool add(const ValueType value);

  std::optional<ValueType> find(const ValueType value);

  bool merge(const ValueType lhs, const ValueType rhs);

private:
  static constexpr size_t kNodesBufferSize{1000};

  std::unordered_map<ValueType, ValueType> m_parents;
};
} // namespace utils
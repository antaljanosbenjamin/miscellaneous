#pragma once

#include <cstdint>
#include <functional>
#include <list>
#include <optional>
#include <unordered_map>

#include "utils/Assert.hpp"

namespace utils {

class DisjointSets {
public:
  using ValueType = int64_t;

  struct Node {
    Node *parent;
    ValueType value;
  };

  [[nodiscard]] int64_t size() const noexcept;

  bool add(const ValueType value);

  std::optional<ValueType> find(const ValueType value);

  bool merge(const ValueType lhs, const ValueType rhs);

private:
  [[nodiscard]] std::optional<std::reference_wrapper<Node>> findNodeByValue(const ValueType value);

  std::list<Node> m_nodes;
  std::unordered_map<ValueType, std::reference_wrapper<Node>> m_valueToNodeMap;
};
} // namespace utils
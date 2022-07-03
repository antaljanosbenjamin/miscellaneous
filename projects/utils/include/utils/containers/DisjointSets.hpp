#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>

#include "utils/Concepts.hpp"

namespace utils {

template <NumericIntegral TValue>
class DisjointSets {
public:
  using ValueType = TValue;

  [[nodiscard]] int64_t size() const noexcept {
    return static_cast<int64_t>(this->m_parents.size());
  }

  bool add(const ValueType value) {
    if (this->m_parents.contains(value)) {
      return false;
    }
    this->m_parents.emplace(value, value);
    return true;
  }

  std::optional<ValueType> find(const ValueType value) {
    if (auto it = this->m_parents.find(value); it != this->m_parents.end()) {
      return this->findParent(it)->first;
    }
    return std::nullopt;
  }

  bool merge(const ValueType lhs, const ValueType rhs) {
    auto lhsIts = this->m_parents.find(lhs);
    if (lhsIts == this->m_parents.end()) {
      return false;
    }
    auto rhsIts = this->m_parents.find(rhs);
    if (rhsIts == this->m_parents.end()) {
      return false;
    }

    auto lhsParent = this->findParent(lhsIts);
    auto rhsParent = this->findParent(rhsIts);
    if (&lhsParent == &rhsParent) {
      return false;
    }

    if (lhsParent->first < rhsParent->first) {
      rhsParent->second = lhsParent->first;
    } else {
      lhsParent->second = rhsParent->first;
    }
    return true;
  }

private:
  using NodeIterator = typename std::unordered_map<ValueType, ValueType>::iterator;
  [[nodiscard]] NodeIterator findParent(NodeIterator it) {

    std::vector<NodeIterator> visitedNodesIts;

    while (it->first != it->second) {
      visitedNodesIts.push_back(it);
      it = this->m_parents.find(it->second);
    }

    for (auto &visitedNodeIt: visitedNodesIts) {
      visitedNodeIt->second = it->second;
    }
    return it;
  }
  std::unordered_map<ValueType, ValueType> m_parents;
};
} // namespace utils
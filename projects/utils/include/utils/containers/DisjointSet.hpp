#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>

#include "utils/Concepts.hpp"

namespace utils::containers {

template <NumericIntegral TValue>
class DisjointSet {
public:
  using ValueType = TValue;

  [[nodiscard]] int64_t size() const noexcept {
    return static_cast<int64_t>(m_parents.size());
  }

  [[nodiscard]] int64_t numberOfDisjointSets() const {
    int64_t counter{0};
    for (const auto &[value, parent]: this->m_parents) {
      if (value == parent) {
        ++counter;
      }
    }
    return counter;
  }

  bool add(const ValueType value) {
    if (m_parents.contains(value)) {
      return false;
    }
    m_parents.emplace(value, value);
    return true;
  }

  [[nodiscard]] std::optional<ValueType> find(const ValueType value) {
    if (auto it = m_parents.find(value); it != m_parents.end()) {
      return this->findParent(it)->first;
    }
    return std::nullopt;
  }

  [[nodiscard]] std::optional<ValueType> find(const ValueType value) const {
    if (auto it = m_parents.find(value); it != m_parents.end()) {
      return this->findParent(it)->first;
    }
    return std::nullopt;
  }

  bool merge(const ValueType lhs, const ValueType rhs) {
    auto lhsIts = m_parents.find(lhs);
    if (lhsIts == m_parents.end()) {
      return false;
    }
    auto rhsIts = m_parents.find(rhs);
    if (rhsIts == m_parents.end()) {
      return false;
    }
    if (lhsIts->second == rhsIts->second) {
      return false;
    }

    auto lhsParent = this->findParent(lhsIts);
    auto rhsParent = this->findParent(rhsIts);
    if (lhsParent == rhsParent) {
      return false;
    }

    if (lhsParent->first < rhsParent->first) {
      rhsParent->second = lhsParent->first;
      rhsIts->second = lhsParent->first;
    } else {
      lhsParent->second = rhsParent->first;
      lhsIts->second = rhsParent->first;
    }
    return true;
  }

private:
  using HashMap = std::unordered_map<ValueType, ValueType>;
  using NodeIterator = typename HashMap::iterator;
  using ConstNodeIterator = typename HashMap::const_iterator;

  [[nodiscard]] NodeIterator findParent(NodeIterator it) {
    auto originalIt = it;
    while (it->first != it->second) {
      it = m_parents.find(it->second);
    }
    originalIt->second = it->second;
    return it;
  }

  [[nodiscard]] ConstNodeIterator findParent(ConstNodeIterator it) const {
    while (it->first != it->second) {
      it = m_parents.find(it->second);
    }

    return it;
  }

  HashMap m_parents;
};
} // namespace utils::containers

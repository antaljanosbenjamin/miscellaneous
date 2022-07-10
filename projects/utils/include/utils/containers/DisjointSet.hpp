#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>

#include "utils/Concepts.hpp"

namespace utils::containers {

// A single implementation of a disjoint-set data structure: https://en.wikipedia.org/wiki/Disjoint-set_data_structure
// It optimized for adding and merging disjoint sets represented by integer numbers. It also support querying the lowest
// integer value in the set to which the specified number belongs.
template <NumericIntegral TValue>
class DisjointSet {
public:
  using ValueType = TValue;

  // Returns the number of **all** sets added to the data structure.
  // Complexity: constant (same as the underlying container)
  [[nodiscard]] int64_t size() const noexcept {
    return static_cast<int64_t>(m_parents.size());
  }

  // Returns the number of disjoint sets in the data structure.
  // Complexity: linear in the size of the data structure (same as iterating over the items of the underlying
  // container)
  [[nodiscard]] int64_t numberOfDisjointSets() const {
    int64_t counter{0};
    for (const auto &[value, parent]: this->m_parents) {
      if (value == parent) {
        ++counter;
      }
    }
    return counter;
  }

  // Adds a new set to the data structure.
  // Returns true if the set is added, or false if the data structure already contains the set.
  // Complexity: amortized constant on average, worst case linear in the size of the data structure (same as adding a
  // new item to the underlying container)
  bool add(const ValueType value) {
    if (m_parents.contains(value)) {
      return false;
    }
    m_parents.emplace(value, value);
    return true;
  }

  // Find the lowest value in the set to which the looked-up value belongs to.
  // Complexity: it is hard to give a theoretical answer and also not very useful. In practice the complexity of the
  // first look-up can be linear in the size of the set of the looked-up value. The result of the look-up is then stored
  // for the looked-up value, making the subsequent lookups faster.
  // As the current underlying container provides amortized constant lookup time on average, it means in best case the
  // find has amortized constant complexity.
  [[nodiscard]] std::optional<ValueType> find(const ValueType value) {
    if (auto it = m_parents.find(value); it != m_parents.end()) {
      return this->findParent(it)->first;
    }
    return std::nullopt;
  }

  // The same stands as above, but the result of the lookup cannot be stored because it is a constant member function.
  [[nodiscard]] std::optional<ValueType> find(const ValueType value) const {
    if (auto it = m_parents.find(value); it != m_parents.end()) {
      return this->findParent(it)->first;
    }
    return std::nullopt;
  }

  // Merges two (probably) disjoint sets.
  // Returns true if the merge happened, and returns false if the two value already belong to the same set or any of the
  // values are absent from the data structure.
  // Complexity: the same as two `find` operations
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

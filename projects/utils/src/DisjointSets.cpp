#include "utils/DisjointSets.hpp"

#include <numeric>
#include <vector>

namespace utils {

namespace {
using ValueType = DisjointSets::ValueType;
using NoteIterator = typename std::unordered_map<ValueType, ValueType>::iterator;
NoteIterator findParent(std::unordered_map<ValueType, ValueType> &parents, NoteIterator it) {

  std::vector<typename std::unordered_map<ValueType, ValueType>::iterator> visitedNodesIts;

  while (it->first != it->second) {
    visitedNodesIts.push_back(it);
    it = parents.find(it->second);
  }

  for (auto &visitedNodeIt: visitedNodesIts) {
    visitedNodeIt->second = it->second;
  }
  return it;
}
} // namespace

[[nodiscard]] int64_t DisjointSets::size() const noexcept {
  return static_cast<int64_t>(this->m_parents.size());
}

bool DisjointSets::add(const ValueType value) {
  if (this->m_parents.contains(value)) {
    return false;
  }
  this->m_parents.emplace(value, value);
  return true;
}

std::optional<DisjointSets::ValueType> DisjointSets::find(const ValueType value) {
  if (auto it = this->m_parents.find(value); it != this->m_parents.end()) {
    return findParent(this->m_parents, it)->first;
  }
  return std::nullopt;
}

bool DisjointSets::merge(const ValueType lhs, const ValueType rhs) {
  auto lhsIts = this->m_parents.find(lhs);
  if (lhsIts == this->m_parents.end()) {
    return false;
  }
  auto rhsIts = this->m_parents.find(rhs);
  if (rhsIts == this->m_parents.end()) {
    return false;
  }

  auto lhsParent = findParent(this->m_parents, lhsIts);
  auto rhsParent = findParent(this->m_parents, rhsIts);
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
} // namespace utils
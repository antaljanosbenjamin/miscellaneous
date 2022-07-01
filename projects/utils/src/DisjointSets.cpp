#include "utils/DisjointSets.hpp"

#include <numeric>
#include <vector>

namespace utils {

namespace {
DisjointSets::Node &findParent(DisjointSets::Node *node) {

  if (node->parent == node) {
    return *node;
  }

  std::vector<DisjointSets::Node *> visitedNodes;
  auto *parentNode = node;
  while (parentNode->parent != parentNode) {
    visitedNodes.push_back(parentNode);
    parentNode = parentNode->parent;
  }

  for (auto *visitedNode: visitedNodes) {
    visitedNode->parent = parentNode;
  }
  return *parentNode;
}
} // namespace

[[nodiscard]] int64_t DisjointSets::size() const noexcept {
  return static_cast<int64_t>(this->m_valueToNodeMap.size());
}

bool DisjointSets::add(const ValueType value) {
  if (this->m_valueToNodeMap.contains(value)) {
    return false;
  }
  auto &node = this->createNode();
  node.parent = &node;
  node.value = value;
  this->m_valueToNodeMap.emplace(value, node);
  return true;
}

std::optional<DisjointSets::ValueType> DisjointSets::find(const ValueType value) {
  auto maybe_node = this->findNodeByValue(value);
  if (!maybe_node.has_value()) {
    return std::nullopt;
  }

  return findParent(&maybe_node->get()).value;
}

bool DisjointSets::merge(const ValueType lhs, const ValueType rhs) {
  auto maybeLhs = this->findNodeByValue(lhs);
  if (!maybeLhs.has_value()) {
    return false;
  }
  auto maybeRhs = this->findNodeByValue(rhs);
  if (!maybeRhs.has_value()) {
    return false;
  }

  auto &lhsParent = findParent(&maybeLhs->get());
  auto &rhsParent = findParent(&maybeRhs->get());
  if (&lhsParent == &rhsParent) {
    return false;
  }

  if (lhsParent.value < rhsParent.value) {
    rhsParent.parent = &lhsParent;
  } else {
    lhsParent.parent = &rhsParent;
  }
  return true;
}

[[nodiscard]] std::optional<std::reference_wrapper<DisjointSets::Node>>
DisjointSets::findNodeByValue(const ValueType value) {

  const auto it = m_valueToNodeMap.find(value);
  if (it == this->m_valueToNodeMap.end()) {
    return std::nullopt;
  }
  return it->second;
}

[[nodiscard]] DisjointSets::Node &DisjointSets::createNode() {
  if (this->m_nodes.empty() || this->m_nodes.back().size() == kNodesBufferSize) {
    m_nodes.emplace_back().reserve(kNodesBufferSize);
  }

  return this->m_nodes.back().emplace_back();
}

} // namespace utils
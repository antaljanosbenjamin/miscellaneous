#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

struct Node {
  std::vector<int> rowIds;
};

int solution(std::vector<std::vector<int>> &A) {
  const auto createInitialNode = [&A]() {
    Node node;
    node.rowIds.resize(A.size());
    std::iota(node.rowIds.begin(), node.rowIds.end(), 0);
    return node;
  };

  std::vector<Node> nodesToProcess{};
  nodesToProcess.push_back(createInitialNode());
  std::vector<Node> newNodes{};

  size_t actualColumnIndex = 0U;
  const auto processNode = [&actualColumnIndex, &A](const Node &node, std::vector<Node> &newNodes) {
    Node changedRows{};
    Node sameRows{};
    for (const auto &rowId: node.rowIds) {
      const auto &row = A[rowId];
      const auto *item = &row[actualColumnIndex];
      if (*item == *(item + 1)) {
        sameRows.rowIds.push_back(rowId);
      } else {
        changedRows.rowIds.push_back(rowId);
      }
    }
    if (!changedRows.rowIds.empty()) {
      newNodes.push_back(std::move(changedRows));
    }
    if (!sameRows.rowIds.empty()) {
      newNodes.push_back(std::move(sameRows));
    }
  };

  const auto nColumns = A[0].size();
  while (actualColumnIndex < nColumns - 1) {
    for (const auto &node: nodesToProcess) {
      processNode(node, newNodes);
    }
    newNodes.swap(nodesToProcess);
    newNodes.clear();
    ++actualColumnIndex;
  }
  const auto nodeWithMostRows =
      std::max_element(nodesToProcess.begin(), nodesToProcess.end(),
                       [](const Node &lhs, const Node &rhs) { return lhs.rowIds.size() < rhs.rowIds.size(); });
  return static_cast<int>(nodeWithMostRows->rowIds.size());
}

int main() {
  {
    std::vector<std::vector<int>> input{
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {1, 0, 1, 1},
    };
    std::cout << solution(input) << '\n';
  }
  {
    std::vector<std::vector<int>> input{
        {0, 1, 0, 1},
        {1, 0, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 1, 0},
    };
    std::cout << solution(input) << '\n';
  }
  return 0;
}
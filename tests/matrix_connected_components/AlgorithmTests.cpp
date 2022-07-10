#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include "matrix_connected_components/Algorithm.hpp"
#include "utils/containers/Matrix.hpp"

#include "MatrixUtils.hpp"

namespace matrix_connected_components::tests {

[[nodiscard]] utils::containers::Matrix<uint64_t> makeInputMatrix(const std::vector<std::string> &input) {
  const auto height = static_cast<int64_t>(input.size());
  const auto width = static_cast<int64_t>(input.front().size());

  utils::containers::Matrix<uint64_t> matrix{height, width, matrix_connected_components::kUnmarkedField<uint64_t>};
  for (int64_t row{0}; row < height; ++row) {
    for (int64_t column{0}; column < width; ++column) {
      if (input[asSizeT(row)][asSizeT(column)] == 'x') {
        matrix.get(row, column) = matrix_connected_components::kMarkedField<uint64_t>;
      }
    }
  }
  return matrix;
}

[[nodiscard]] int64_t numberOfConnectedComponents(const std::unordered_map<uint64_t, uint64_t> &labelSets) {
  int64_t count{0};
  for (const auto [label, root]: labelSets) {
    if (label == root) {
      ++count;
    }
  }
  return count;
}

void checkMatrix(const std::vector<std::vector<uint64_t>> &expected,
                 const utils::containers::Matrix<uint64_t> &actual) {
  REQUIRE(expected.size() == asSizeT(actual.height()));
  REQUIRE(expected.front().size() == asSizeT(actual.width()));
  for (int64_t row{0}; row < actual.height(); ++row) {
    for (int64_t column{0}; column < actual.width(); ++column) {
      INFO("Row: " << row << ", column: " << column);
      CHECK(expected[asSizeT(row)][asSizeT(column)] == actual.get(row, column));
    }
  }
}

void checkLabelUnions(const std::unordered_map<uint64_t, uint64_t> &expected, const DisjointSet<uint64_t> &actual) {
  CHECK(expected.size() == asSizeT(actual.size()));
  for (const auto [label, root]: expected) {
    const auto rootFromActual = actual.find(label);
    REQUIRE(rootFromActual.has_value());
    CHECK(root == *rootFromActual);
  }
}

static_assert(0 == matrix_connected_components::kUnmarkedField<uint64_t>);
static_assert(1 == matrix_connected_components::kMarkedField<uint64_t>);

struct TestCase {
  std::string name;
  std::vector<std::string> input;
  std::vector<std::vector<uint64_t>> expectedInitialLabels;
  std::unordered_map<uint64_t, uint64_t> expectedLabelUnions;
  std::vector<std::vector<uint64_t>> expectedConnectedComponents;
};

TEST_CASE("Generic") {
  std::vector<TestCase>
      testCases =
          {
              {
                  .name = "EmptyMatrix",
                  .input =
                      {
                          {"..."},
                          {"..."},
                      },
                  .expectedInitialLabels =
                      {
                          {0, 0, 0},
                          {0, 0, 0},
                      },
                  .expectedLabelUnions = {},
                  .expectedConnectedComponents =
                      {
                          {0, 0, 0},
                          {0, 0, 0},
                      },
              },
              {
                  .name = "FullMatrix",
                  .input =
                      {
                          {"xx"},
                          {"xx"},
                          {"xx"},
                          {"xx"},
                      },
                  .expectedInitialLabels =
                      {
                          {2, 2},
                          {2, 2},
                          {2, 2},
                          {2, 2},
                      },
                  .expectedLabelUnions =
                      {
                          {2, 2},
                      },
                  .expectedConnectedComponents =
                      {
                          {2, 2},
                          {2, 2},
                          {2, 2},
                          {2, 2},
                      },
              },
              {
                  .name = "Example",
                  .input =
                      {
                          {"x...."},
                          {"x.xx."},
                          {".x.x."},
                          {".xxx."},
                          {"....x"},
                      },
                  .expectedInitialLabels =
                      {
                          {2, 0, 0, 0, 0},
                          {2, 0, 3, 3, 0},
                          {0, 4, 0, 3, 0},
                          {0, 4, 4, 3, 0},
                          {0, 0, 0, 0, 5}, // NOLINT(readability-magic-numbers)
                      },
                  .expectedLabelUnions =
                      {
                          {2, 2},
                          {3, 3},
                          {4, 3},
                          // NOLINTNEXTLINE(readability-magic-numbers)
                          {5, 5},
                      },
                  .expectedConnectedComponents =
                      {
                          {2, 0, 0, 0, 0},
                          {2, 0, 3, 3, 0},
                          {0, 3, 0, 3, 0},
                          {0, 3, 3, 3, 0},
                          {0, 0, 0, 0, 5}, // NOLINT(readability-magic-numbers)
                      },
              },
              {
                  .name = "MultipleMerge",
                  .input =
                      {
                          {"xxxxx.xxx"},
                          {"....xxx.x"},
                          {"x.x.x.x.x"},
                          {"xxxxxxx.x"},
                          {".x.x..x.x"},
                          {".xxxxxxxx"},
                      },
                  .expectedInitialLabels =
                      {
                          {2, 2, 2, 2, 2, 0, 3, 3, 3},
                          {0, 0, 0, 0, 2, 2, 2, 0, 3},
                          {4, 0, 5, 0, 2, 0, 2, 0, 3}, // NOLINT(readability-magic-numbers)
                          {4, 4, 4, 4, 2, 2, 2, 0, 3},
                          {0, 4, 0, 4, 0, 0, 2, 0, 3},
                          {0, 4, 4, 4, 4, 4, 2, 2, 2},
                      },
                  .expectedLabelUnions =
                      {
                          {2, 2},
                          {3, 2},
                          {4, 2},
                          // NOLINTNEXTLINE(readability-magic-numbers)
                          {5, 2},
                      },
                  .expectedConnectedComponents =
                      {
                          {2, 2, 2, 2, 2, 0, 2, 2, 2},
                          {0, 0, 0, 0, 2, 2, 2, 0, 2},
                          {2, 0, 2, 0, 2, 0, 2, 0, 2},
                          {2, 2, 2, 2, 2, 2, 2, 0, 2},
                          {0, 2, 0, 2, 0, 0, 2, 0, 2},
                          {0, 2, 2, 2, 2, 2, 2, 2, 2},
                      },
              },
              {
                  .name = "SnailPattern",
                  .input =
                      {
                          {"x.xxxx"},
                          {"x.x..x"},
                          {"x.xx.x"},
                          {"x....x"},
                          {"xxxxxx"},
                      },
                  .expectedInitialLabels =
                      {
                          {2, 0, 3, 3, 3, 3},
                          {2, 0, 3, 0, 0, 3},
                          {2, 0, 3, 3, 0, 3},
                          {2, 0, 0, 0, 0, 3},
                          {2, 2, 2, 2, 2, 2},
                      },
                  .expectedLabelUnions =
                      {
                          {2, 2},
                          {3, 2},
                      },
                  .expectedConnectedComponents =
                      {
                          {2, 0, 2, 2, 2, 2},
                          {2, 0, 2, 0, 0, 2},
                          {2, 0, 2, 2, 0, 2},
                          {2, 0, 0, 0, 0, 2},
                          {2, 2, 2, 2, 2, 2},
                      },
              },
              {
                  .name = "ChessTable",
                  .input =
                      {
                          {"x.x.x."},
                          {".x.x.x"},
                          {"x.x.x."},
                          {".x.x.x"},
                      },
                  .expectedInitialLabels =
                      {
                          {2, 0, 3, 0, 4, 0},
                          {0, 5, 0, 6, 0, 7},    // NOLINT(readability-magic-numbers)
                          {8, 0, 9, 0, 10, 0},   // NOLINT(readability-magic-numbers)
                          {0, 11, 0, 12, 0, 13}, // NOLINT(readability-magic-numbers)
                      },
                  .expectedLabelUnions =
                      {
                          {2, 2},
                          {3, 3},
                          {4, 4},
                          {5, 5},   // NOLINT(readability-magic-numbers)
                          {6, 6},   // NOLINT(readability-magic-numbers)
                          {7, 7},   // NOLINT(readability-magic-numbers)
                          {8, 8},   // NOLINT(readability-magic-numbers)
                          {9, 9},   // NOLINT(readability-magic-numbers)
                          {10, 10}, // NOLINT(readability-magic-numbers)
                          {11, 11}, // NOLINT(readability-magic-numbers)
                          {12, 12}, // NOLINT(readability-magic-numbers)
                          {13, 13}, // NOLINT(readability-magic-numbers)
                      },
                  .expectedConnectedComponents =
                      {
                          {2, 0, 3, 0, 4, 0},
                          {0, 5, 0, 6, 0, 7},    // NOLINT(readability-magic-numbers)
                          {8, 0, 9, 0, 10, 0},   // NOLINT(readability-magic-numbers)
                          {0, 11, 0, 12, 0, 13}, // NOLINT(readability-magic-numbers)
                      },
              },
              {
                  .name = "ConcentricCircle",
                  .input =
                      {
                          {"xxxxx"},
                          {"x...x"},
                          {"x.x.x"},
                          {"x...x"},
                          {"xxxxx"},
                      },
                  .expectedInitialLabels =
                      {
                          {2, 2, 2, 2, 2},
                          {2, 0, 0, 0, 2},
                          {2, 0, 3, 0, 2},
                          {2, 0, 0, 0, 2},
                          {2, 2, 2, 2, 2},
                      },
                  .expectedLabelUnions =
                      {
                          {2, 2},
                          {3, 3},
                      },
                  .expectedConnectedComponents =
                      {
                          {2, 2, 2, 2, 2},
                          {2, 0, 0, 0, 2},
                          {2, 0, 3, 0, 2},
                          {2, 0, 0, 0, 2},
                          {2, 2, 2, 2, 2},
                      },
              },
          };

  for (const auto &testCase: testCases) {
    INFO(testCase.name);

    const auto expectedNumberOfConnectedComponents = numberOfConnectedComponents(testCase.expectedLabelUnions);
    auto matrix = makeInputMatrix(testCase.input);
    const auto numberOfConnectedComponent = countConnectedComponents(matrix);
    const auto labelledMatrix = labelConnectedComponents(matrix);
    const auto labelSets = assignInitialLabels(matrix);

    CHECK(expectedNumberOfConnectedComponents == numberOfConnectedComponent);
    {
      INFO("Checking label unions after initial labels");
      checkLabelUnions(testCase.expectedLabelUnions, labelSets);
    }
    {
      INFO("Checking initial labels");
      checkMatrix(testCase.expectedInitialLabels, matrix);
    }
    {
      INFO("Checking labelled matrix");
      checkMatrix(testCase.expectedConnectedComponents, labelledMatrix);
    }
    auto labelSetsCopy = labelSets;
    relabelMatrix(matrix, labelSetsCopy);
    {
      INFO("Checking manually labelled matrix");
      checkMatrix(testCase.expectedConnectedComponents, labelledMatrix);
    }
    {
      INFO("Checking label unions after relabeling");
      checkLabelUnions(testCase.expectedLabelUnions, labelSetsCopy);
    }
  }
}
} // namespace matrix_connected_components::tests

#pragma once

#include <cstdint>
#include <vector>

#include "utils/containers/Matrix.hpp"

namespace matrix_connected_components::tests {

[[nodiscard]] size_t asSizeT(const int64_t value);
[[nodiscard]] utils::containers::Matrix<uint64_t> makeMatrix(const std::vector<std::vector<uint64_t>> &input,
                                                             uint64_t defaultValue);

} // namespace matrix_connected_components::tests

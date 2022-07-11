#pragma once

#include <cstdint>
#include <limits>

#include "utils/Concepts.hpp"

namespace matrix_connected_components {
template <typename TMatrix>
concept HasNumericIntegralValueType = utils::NumericIntegral<typename TMatrix::ValueType>;

template <typename TMatrix>
concept HasGet = requires(const TMatrix &constMatrix, TMatrix &matrix, int64_t index) {
  { constMatrix.get(index, index) } -> std::same_as<const typename TMatrix::ValueType &>;
  { matrix.get(index, index) } -> std::same_as<typename TMatrix::ValueType &>;
};

template <typename TMatrix>
concept IsNumericalMatrixLike = HasNumericIntegralValueType<TMatrix> && HasGet<TMatrix>;
} // namespace matrix_connected_components

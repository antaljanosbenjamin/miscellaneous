#pragma once

#ifdef _WIN32
  #include <propagate_const.h>
#else
  #include <experimental/propagate_const>
#endif

namespace utils {
template <typename T>
using PropagateConst = std::experimental::propagate_const<T>;
} // namespace utils

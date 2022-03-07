#pragma once

namespace utils {
// from https://stackoverflow.com/a/2595226/6639989
template <class T>
inline void hashCombine(std::size_t &seed, const T &v) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace utils

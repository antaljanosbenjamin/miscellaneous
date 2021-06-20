#include "Ruthenium.hpp"

#include <algorithm>
#include <unordered_map>

namespace Ruthenium {

class Interval {
public:
  void add(const int age) {
    m_numberOfBooks++;
    m_counters[age]++;
    if (m_counters[age] > m_counters[m_mostFrequentAge]) {
      m_mostFrequentAge = age;
    }
  };

  void remove(const int age) {
    m_numberOfBooks--;
    m_counters[age]--;
    if (m_counters[age] == 0) {
      m_counters.erase(age);
    }
    if (age == m_mostFrequentAge) {
      m_mostFrequentAge = std::max_element(m_counters.begin(), m_counters.end())->first;
    }
  }

  bool canBeContinuousWithKReplacement(const int K) {
    return (m_numberOfBooks - m_counters[m_mostFrequentAge]) <= K;
  }

  int size() const {
    return m_numberOfBooks;
  }

private:
  std::unordered_map<int, int> m_counters;
  int m_mostFrequentAge{0U};
  int m_numberOfBooks{0U};
};

int solution(std::vector<int> &A, int K) {
  Interval interval{};
  int intervalMaxSize{0};
  for (auto &a: A) {
    --a;
  }
  for (auto addedBook = 0U; addedBook < A.size(); ++addedBook) {
    interval.add(A[addedBook]);
    if (interval.canBeContinuousWithKReplacement(K)) {
      intervalMaxSize = std::max(intervalMaxSize, interval.size());
    } else {
      interval.remove(A[addedBook - interval.size() + 1]);
    }
  }
  return intervalMaxSize;
}

} // namespace Ruthenium

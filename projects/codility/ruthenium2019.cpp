#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

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

int main() {
  {
    std::vector<int> input{1, 1, 3, 4, 3, 3, 4};        // NOLINT(readability-magic-numbers)
    std::cout << "5 == " << solution(input, 2) << '\n'; // NOLINT(readability-magic-numbers)
  }
  {
    std::vector<int> input{4, 5, 5, 4, 2, 2, 4};        // NOLINT(readability-magic-numbers)
    std::cout << "2 == " << solution(input, 0) << '\n'; // NOLINT(readability-magic-numbers)
  }
  {
    std::vector<int> input{1, 3, 3, 2};                 // NOLINT(readability-magic-numbers)
    std::cout << "4 == " << solution(input, 2) << '\n'; // NOLINT(readability-magic-numbers)
  }
  {
    std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // NOLINT(readability-magic-numbers)
    std::cout << "10 == " << solution(input, 10) << '\n';  // NOLINT(readability-magic-numbers)
  }
  return 0;
}
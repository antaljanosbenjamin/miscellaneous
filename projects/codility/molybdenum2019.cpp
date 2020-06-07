#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Interval {
public:
  explicit Interval(const std::vector<int> &input)
    : m_counters{}
    , m_mostFrequentNumber{input[0]}
    , m_nNumbers{static_cast<int>(input.size())} {
    for (const auto &number: input) {
      m_counters[number]++;
    }
    updateMostFrequent();
  }
  void exchangeNumber(const int toRemove, const int toAdd) {
    if (toRemove == toAdd) {
      return;
    }
    m_counters[toAdd]++;
    m_counters[toRemove]--;
    if (m_counters[toAdd] > m_counters[m_mostFrequentNumber]) {
      m_mostFrequentNumber = toAdd;
    } else if (toRemove == m_mostFrequentNumber) {
      updateMostFrequent();
    }
  };

  int getLeader() {
    if (m_counters[m_mostFrequentNumber] > (m_nNumbers / 2)) {
      return m_mostFrequentNumber;
    }
    return -1;
  }

private:
  void updateMostFrequent() {
    m_mostFrequentNumber = std::max_element(m_counters.begin(), m_counters.end(),
                                            [](const std::pair<int, int> &lhs, const std::pair<int, int> &rhs) {
                                              return lhs.second < rhs.second;
                                            })
                               ->first;
  }
  std::unordered_map<int, int> m_counters;
  int m_mostFrequentNumber{0U};
  const int m_nNumbers;
};

std::vector<int> solution(std::vector<int> &A, int K) {
  std::unordered_set<int> result;
  std::for_each(A.begin(), A.begin() + K, [](int &value) { ++value; });
  Interval interval{A};
  result.insert(interval.getLeader());

  for (auto toDecrement = A.begin(), toIncrement = A.begin() + K; toIncrement != A.end();
       ++toDecrement, ++toIncrement) {
    interval.exchangeNumber(*toIncrement, (*toIncrement) + 1);
    ++(*toIncrement);
    interval.exchangeNumber(*toDecrement, (*toDecrement) - 1);
    --(*toDecrement);
    result.insert(interval.getLeader());
  }
  result.erase(-1);
  std::vector<int> resultVector(result.begin(), result.end());
  std::sort(resultVector.begin(), resultVector.end());
  return resultVector;
}

std::ostream &operator<<(std::ostream &os, const std::vector<int> &values) {
  if (values.empty()) {
    os << "[]";
    return os;
  }

  os << '[' << values[0];
  for (auto it = ++values.begin(); it != values.end(); ++it) {
    os << ", " << *it;
  }
  os << ']';
  return os;
}

int main() {
  {
    std::vector<int> input{2, 1, 3, 1, 2, 2, 3};
    std::cout << "[2, 3] == " << solution(input, 3) << '\n';
  }
  {
    std::vector<int> input{1, 2, 2, 1, 2};
    std::cout << "[2, 3] == " << solution(input, 4) << '\n';
  }
  {
    std::vector<int> input{1};
    std::cout << "[2] == " << solution(input, 1) << '\n';
  }
  return 0;
}
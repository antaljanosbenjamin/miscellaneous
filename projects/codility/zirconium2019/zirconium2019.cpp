
int main() {
  {
    std::vector<int> A{4, 2, 1};                        // NOLINT(readability-magic-numbers)
    std::vector<int> B{2, 5, 3};                        // NOLINT(readability-magic-numbers)
    std::cout << "10 == " << solution(A, B, 2) << '\n'; // NOLINT(readability-magic-numbers)
  }
  {
    std::vector<int> A{7, 1, 4, 4};                     // NOLINT(readability-magic-numbers)
    std::vector<int> B{5, 3, 4, 3};                     // NOLINT(readability-magic-numbers)
    std::cout << "18 == " << solution(A, B, 2) << '\n'; // NOLINT(readability-magic-numbers)
  }
  {
    std::vector<int> A{5, 5, 5};                        // NOLINT(readability-magic-numbers)
    std::vector<int> B{5, 5, 5};                        // NOLINT(readability-magic-numbers)
    std::cout << "15 == " << solution(A, B, 1) << '\n'; // NOLINT(readability-magic-numbers)
  }
  return 0;
}
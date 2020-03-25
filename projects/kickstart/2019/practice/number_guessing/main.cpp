#include <iostream>
#include <string>

int main() {
  size_t T;
  std::cin >> T;
  for (auto i{0u}; i < T; ++i) {
    size_t A, B;
    std::cin >> A >> B;
    ++A;
    size_t N;
    std::cin >> N;
    while (true) {
      size_t Q{(A + B) / 2};
      std::cout << Q << std::endl;
      std::string answer;
      std::cin >> answer;
      if (answer == "WRONG_ANSWER" || answer == "CORRECT") {
        break;
      } else if (answer == "TOO_SMALL") {
        A = Q + 1;
      } else if (answer == "TOO_BIG") {
        B = Q - 1;
      }
    }
  }
}
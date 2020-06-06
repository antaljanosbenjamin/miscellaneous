#include <iostream>
#include <string>

int main() {
  size_t T{0};
  std::cin >> T;
  for (auto i{0U}; i < T; ++i) {
    size_t A{0};
    size_t B{0};
    std::cin >> A >> B;
    ++A;
    size_t N{0};
    std::cin >> N;
    while (true) {
      size_t Q{(A + B) / 2};
      std::cout << Q << std::endl;
      std::string answer;
      std::cin >> answer;
      if (answer == "WRONG_ANSWER" || answer == "CORRECT") {
        break;
      }
      if (answer == "TOO_SMALL") {
        A = Q + 1;
      } else if (answer == "TOO_BIG") {
        B = Q - 1;
      }
    }
  }
}
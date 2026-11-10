#include "Algo.cpp"

#include <cmath>
#include <iostream>

int main() {
    static double risk[100][100]{};
    risk[0][1] = 0.1;
    risk[1][2] = 0.2;

    const double empty = probabilisticPathSafety(risk, {});
    const double single = probabilisticPathSafety(risk, {0});
    const double forward = probabilisticPathSafety(risk, {0, 1, 2});
    const double reverse = probabilisticPathSafety(risk, {2, 1, 0});

    std::cout << "Empty | P_safe=" << empty << '\n'
              << "0 | P_safe=" << single << '\n'
              << "0 -> 1 -> 2 | P_safe=" << forward << '\n'
              << "2 -> 1 -> 0 | P_safe=" << reverse << '\n';

    if (empty != 1.0 || single != 1.0 ||
        std::abs(forward - 0.72) >= 1e-12 || reverse != 1.0) {
        return 1;
    }

    std::cout << "PASS\n";
    return 0;
}

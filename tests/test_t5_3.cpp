#include "Algo.cpp"

#include <iostream>

int main() {
    const int down = 1;
    const int right = 3;
    const double first = pcbStepCost(1.0, PCBStateKey::None, right);
    const double straight = pcbStepCost(1.0, right, right);
    const double turn = pcbStepCost(1.0, right, down);

    std::cout << "First=" << first << " | Straight=" << straight
              << " | Turn=" << turn << '\n';
    if (first != 1.0 || straight != 1.0 || turn != 1.5 ||
        pcbStepCost(2.0, right, down) != 2.5) {
        return 1;
    }

    const int cols = 4;
    const int start = 0;
    const int goal = 11;
    const double manhattan = pcbHeuristic(start, goal, cols, 1);
    const double chebyshev = pcbHeuristic(start, goal, cols, 2);
    std::cout << "Manhattan=" << manhattan
              << " | Chebyshev=" << chebyshev << '\n';
    if (manhattan != 5.0 || chebyshev != 3.0 ||
        pcbHeuristic(goal, goal, cols, 1) != 0.0 ||
        pcbHeuristic(goal, goal, cols, 2) != 0.0) {
        return 1;
    }

    std::cout << "PASS\n";
    return 0;
}

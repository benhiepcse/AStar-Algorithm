#include "Algo.cpp"

#include <iostream>

int main() {
    const GameDirection right{0, 1, "Right"};
    const GameDirection downRight{1, 1, "Down-Right"};
    const double flatStraight = gameStepCost(0, right);
    const double swampStraight = gameStepCost(2, right);
    const double flatDiagonal = gameStepCost(0, downRight);
    const double swampDiagonal = gameStepCost(2, downRight);

    std::cout << "Flat straight=" << flatStraight << '\n'
              << "Swamp straight=" << swampStraight << '\n'
              << "Flat diagonal=" << flatDiagonal << '\n'
              << "Swamp diagonal=" << swampDiagonal << '\n';
    if (flatStraight != 1.0 || swampStraight != 2.0 ||
        flatDiagonal != 1.5 || swampDiagonal != 3.0) {
        return 1;
    }

    const double manhattan = gameHeuristic(0, 0, 1, 1, 1);
    const double chebyshev = gameHeuristic(0, 0, 1, 1, 2);
    std::cout << "Manhattan=" << manhattan << " | Chebyshev=" << chebyshev
              << " | actual diagonal cost=" << flatDiagonal << '\n';

    if (manhattan != 2.0 || chebyshev != 1.0 ||
        gameHeuristic(1, 1, 1, 1, 1) != 0.0 ||
        gameHeuristic(1, 1, 1, 1, 2) != 0.0) {
        return 1;
    }

    std::cout << "PASS\n";
    return 0;
}

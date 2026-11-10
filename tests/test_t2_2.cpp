#include "Algo.cpp"

#include <cmath>
#include <iostream>

int main() {
    const int from[3] = {1, 2, 30};
    const int goal[3] = {4, 6, 18};
    const double expected[3] = {19.0, 13.0, 12.0};

    for (int mode = 1; mode <= 3; ++mode) {
        const double actual = auvHeuristic(from, goal, mode);
        const double atGoal = auvHeuristic(goal, goal, mode);
        std::cout << "Mode " << mode << " | h=" << actual
                  << " | h(goal)=" << atGoal << '\n';

        if (!(std::abs(actual - expected[mode - 1]) < 1e-9) ||
            atGoal != 0.0) {
            return 1;
        }
    }

    if (auvHeuristic(from, goal, 99) != 0.0) {
        return 1;
    }

    std::cout << "PASS\n";
    return 0;
}

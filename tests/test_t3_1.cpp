#include "Algo.cpp"

#include <iostream>

int main() {
    const double noPenalty = probabilisticEdgeCost(10.0, 0.5, 0.0);
    const double lowRisk = probabilisticEdgeCost(10.0, 0.125, 8.0);
    const double highRisk = probabilisticEdgeCost(10.0, 0.5, 8.0);

    std::cout << "lambda=0 | cost=" << noPenalty << '\n';
    std::cout << "lambda=8 | low=" << lowRisk << " | high=" << highRisk << '\n';
    if (noPenalty != 10.0 || lowRisk != 11.0 || highRisk != 14.0) {
        return 1;
    }

    const int from[2] = {1, 6};
    const int goal[2] = {4, 2};
    const double expected[] = {7.0, 5.0};

    for (int mode = 1; mode <= 2; ++mode) {
        const double h = probabilisticHeuristic(from, goal, mode);
        const double atGoal = probabilisticHeuristic(goal, goal, mode);
        std::cout << "Mode " << mode << " | h=" << h
                  << " | h(goal)=" << atGoal << '\n';
        if (h != expected[mode - 1] || atGoal != 0.0) {
            return 1;
        }
    }

    std::cout << "PASS\n";
    return 0;
}

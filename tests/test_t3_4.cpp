#include "Algo.h"

#include <cmath>
#include <iostream>

int main() {
    static double dist[100][100]{};
    static double risk[100][100]{};
    static int coords[100][2]{};
    coords[1][0] = 1;
    coords[2][0] = 2;

    dist[0][2] = 4.0;
    risk[0][2] = 0.5;
    dist[0][1] = dist[1][2] = 3.0;
    risk[0][1] = risk[1][2] = 0.125;

    const struct {
        double lambda;
        bool viaOne;
        double cost;
        double safety;
    } cases[] = {
        {4.0, false, 6.0, 0.5},
        {8.0, false, 8.0, 0.5},
        {12.0, true, 9.0, 0.765625},
    };

    for (int mode = 1; mode <= 2; ++mode) {
        for (const auto& test : cases) {
            PathNode* path = findProbabilisticPath(
                dist, risk, coords, 0, 2, test.lambda, mode);
            const PathNode* next = path ? path->next : nullptr;
            const PathNode* goal = test.viaOne && next ? next->next : next;
            const double safety = getLastProbabilisticPathSafety();

            const bool ok = path && path->name == "0" && next &&
                            (!test.viaOne || next->name == "1") &&
                            goal && goal->name == "2" && !goal->next &&
                            std::abs(goal->g - test.cost) < 1e-9 &&
                            std::abs(safety - test.safety) < 1e-9;
            if (ok) {
                std::cout << "Mode " << mode << " | lambda=" << test.lambda
                          << " | 0 -> " << (test.viaOne ? "1 -> " : "")
                          << "2 | g=" << goal->g << " | P_safe=" << safety << '\n';
            }
            freePath(path);
            if (!ok) return 1;
        }
    }

    std::cout << "PASS\n";
    return 0;
}

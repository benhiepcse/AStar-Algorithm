#include "Algo.h"

#include <cmath>
#include <iostream>

bool near(double a, double b) {
    return std::abs(a - b) < 1e-9;
}

bool matches(const PathNode* node, const char* name, double g, double h) {
    return node != nullptr && node->name == name && near(node->g, g) &&
           near(node->h, h) && near(node->f, g + h);
}

int main() {
    static double dist[100][100]{};
    static double risk[100][100]{};
    static int coords[100][2]{};
    coords[1][0] = 1;
    coords[2][0] = 2;
    coords[2][1] = 1;
    dist[0][1] = 4;
    dist[1][2] = 3;
    dist[0][2] = 12;
    risk[0][1] = 0.1;
    risk[1][2] = 0.2;

    for (int mode = 1; mode <= 2; ++mode) {
        PathNode* path = findProbabilisticPath(dist, risk, coords, 0, 2, 10, mode);
        const double safety = getLastProbabilisticPathSafety();
        const bool ok = matches(path, "0", 0, mode == 1 ? 3 : std::sqrt(5.0)) &&
                        matches(path->next, "1", 5, mode == 1 ? 2 : std::sqrt(2.0)) &&
                        matches(path->next->next, "2", 10, 0) &&
                        path->next->next->next == nullptr && near(safety, 0.72);
        freePath(path);
        if (!ok) return 1;
        std::cout << "Mode " << mode << " | 0 -> 1 -> 2 | g=10 | P_safe="
                  << safety << '\n';
    }

    PathNode* path = findProbabilisticPath(dist, risk, coords, 2, 2, 10, 1);
    const bool sameOk = matches(path, "2", 0, 0) && path->next == nullptr &&
                        getLastProbabilisticPathSafety() == 1.0;
    freePath(path);
    if (!sameOk) return 1;

    risk[1][2] = 1.0;
    dist[0][2] = 0.0;
    path = findProbabilisticPath(dist, risk, coords, 0, 2, 10, 1);
    const bool failed = path == nullptr && getLastProbabilisticPathSafety() == -1.0;
    freePath(path);
    if (!failed) return 1;

    std::cout << "PASS\n";
    return 0;
}

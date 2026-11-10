#include "Algo.h"

#include <cmath>
#include <iostream>

bool matches(const PathNode* node, const char* name, double g, double h) {
    return node != nullptr && node->name == name &&
           std::abs(node->g - g) < 1e-9 &&
           std::abs(node->h - h) < 1e-9 &&
           std::abs(node->f - (g + h)) < 1e-9;
}

int main() {
    static double weights[100][100]{};
    static int coords[100][3]{};
    coords[1][2] = 100;
    coords[2][0] = 3;
    coords[2][1] = 4;
    coords[2][2] = 100;
    weights[0][1] = 100;
    weights[1][2] = 10;
    weights[0][2] = 150;

    const double startH[] = {107, std::sqrt(10025.0), 100};
    const double middleH[] = {7, 5, 4};
    for (int mode = 1; mode <= 3; ++mode) {
        PathNode* path = findAUVPath(weights, coords, 0, 2, mode);
        const bool ok = matches(path, "0", 0, startH[mode - 1]) &&
                        matches(path->next, "1", 200, middleH[mode - 1]) &&
                        matches(path->next->next, "2", 210, 0) &&
                        path->next->next->next == nullptr;
        if (ok) {
            std::cout << "Mode " << mode << " | 0 -> 1 -> 2 | g="
                      << path->next->next->g << '\n';
        }
        freePath(path);
        if (!ok) return 1;
    }

    PathNode* same = findAUVPath(weights, coords, 1, 1, 2);
    const bool sameOk = matches(same, "1", 0, 0) && same->next == nullptr;
    freePath(same);
    if (!sameOk) return 1;

    PathNode* cases[] = {
        findAUVPath(weights, coords, 2, 0, 1),
        findAUVPath(weights, coords, -1, 2, 1),
        findAUVPath(weights, coords, 0, 100, 1),
        findAUVPath(weights, coords, 0, 2, 99),
        findAUVPath(nullptr, coords, 0, 2, 1),
        findAUVPath(weights, nullptr, 0, 2, 1),
    };
    bool casesOk = true;
    for (PathNode* path : cases) {
        if (path != nullptr) casesOk = false;
        freePath(path);
    }
    if (!casesOk) return 1;

    std::cout << "PASS\n";
    return 0;
}

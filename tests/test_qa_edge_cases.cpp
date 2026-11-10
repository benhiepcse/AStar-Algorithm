#include "Algo.h"

#include <iostream>

bool absent(PathNode* path) {
    const bool ok = path == nullptr;
    freePath(path);
    return ok;
}

bool line(PathNode* path, int count, double goalCost) {
    int length = 0;
    bool ok = true;
    for (const PathNode* node = path; node && length < count; node = node->next) {
        ++length;
        ok = ok && node->f == node->g + node->h;
        if (length == count) {
            ok = ok && !node->next && node->g == goalCost;
        }
    }
    freePath(path);
    return ok && length == count;
}

int main() {
    static double weights[100][100]{}, risk[100][100]{}, pcb[100][100]{};
    static int xyz[100][3]{}, xy[100][2]{}, grid[100][100]{};

    weights[0][99] = 2.0;
    if (!line(findMeshPath(weights, 0, 99), 2, 2.0) ||
        !absent(findMeshPath(nullptr, 0, 99)) ||
        !absent(findMeshPath(weights, 0, 100)) ||
        !absent(findAUVPath(weights, xyz, 0, 99, 4)) ||
        !absent(findProbabilisticPath(weights, risk, xy, 0, 99, -1, 1)) ||
        !absent(findGamePath(grid, 0, 1, 0, 0, 0, 0, 1)) ||
        !absent(findPCBPath(grid, 11, 10, 0, 0, 0, 1, pcb, 1)) ||
        !line(findPCBPath(grid, 100, 1, 0, 0, 99, 0, pcb, 1), 100, 99) ||
        pcb[98][99] != 1.0 ||
        !line(findGamePath(grid, 100, 100, 0, 0, 99, 99, 2), 100, 148.5)) {
        std::cerr << "FAIL: boundary and defensive checks\n";
        return 1;
    }

    std::cout << "PASS: boundary and defensive checks\n";
    return 0;
}

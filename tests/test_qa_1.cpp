#include "Algo.h"

#include <cmath>
#include <iostream>

bool check(PathNode* path, const char* first, const char* last = nullptr,
           double cost = 0, double h = 0) {
    const PathNode* next = path ? path->next : nullptr;
    bool ok = !first ? !path : path && path->name == first &&
              path->g == 0 && path->h == h && path->f == h;
    ok = ok && (!last ? !next : next && next->name == last &&
                !next->next && std::abs(next->g - cost) < 1e-9 &&
                next->h == 0 && next->f == next->g);
    freePath(path);
    return ok;
}

int main() {
    static double w[100][100]{}, risk[100][100]{}, pcb[100][100]{};
    static int xyz[100][3]{}, xy[100][2]{}, grid[100][100]{};
    w[0][1] = 2;
    xyz[1][2] = 100;
    xy[1][0] = 1;
    risk[0][1] = 0.25;

    if (!check(findMeshPath(w, 0, 1), "0", "1", 2, 2) ||
        !check(findMeshPath(w, 0, 0), "0") ||
        !check(findMeshPath(w, 1, 0), nullptr)) return 1;

    for (int mode = 1; mode <= 3; ++mode) {
        if (!check(findAUVPath(w, xyz, 0, 1, mode), "0", "1", 4, 100) ||
            !check(findAUVPath(w, xyz, 0, 0, mode), "0") ||
            !check(findAUVPath(w, xyz, 1, 0, mode), nullptr)) return 1;
    }

    for (int mode = 1; mode <= 2; ++mode) {
        if (!check(findProbabilisticPath(w, risk, xy, 0, 1, 4, mode),
                   "0", "1", 3, 1) || getLastProbabilisticPathSafety() != 0.75 ||
            !check(findProbabilisticPath(w, risk, xy, 0, 0, 4, mode), "0") ||
            getLastProbabilisticPathSafety() != 1 ||
            !check(findProbabilisticPath(w, risk, xy, 1, 0, 4, mode), nullptr) ||
            getLastProbabilisticPathSafety() != -1) return 1;

        grid[0][1] = 2;
        if (!check(findGamePath(grid, 1, 2, 0, 0, 0, 1, mode),
                   "Start", "Right", 2, 1) ||
            !check(findGamePath(grid, 1, 2, 0, 0, 0, 0, mode), "Start")) return 1;
        grid[0][1] = grid[1][0] = 1;
        if (!check(findGamePath(grid, 2, 2, 0, 0, 1, 1, mode), nullptr)) return 1;
        grid[0][1] = grid[1][0] = 0;

        if (!check(findPCBPath(grid, 1, 2, 0, 0, 0, 1, pcb, mode),
                   "(0,0)", "(0,1)", 1, 1) ||
            pcb[0][1] != 1 || pcb[1][0] != 1 ||
            !check(findPCBPath(grid, 1, 2, 0, 0, 0, 0, pcb, mode), "(0,0)")) return 1;
        grid[0][1] = 1;
        if (!check(findPCBPath(grid, 1, 3, 0, 0, 0, 2, pcb, mode), nullptr) ||
            pcb[0][1] != 0 || pcb[1][2] != 0) return 1;
    }

    std::cout << "PASS: 30 API cases\n";
    return 0;
}

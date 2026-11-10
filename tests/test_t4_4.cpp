#include "Algo.h"

#include <iostream>

int main() {
    static int grid[100][100]{};
    grid[1][1] = grid[1][2] = 2;
    grid[2][0] = 1;

    const char* labels[] = {"Start", "Up-Right", "Right", "Down-Right"};
    const double costs[] = {0.0, 1.5, 2.5, 4.0};
    const double heuristics[][4] = {{3, 3, 2, 0}, {3, 2, 1, 0}};

    for (int mode = 1; mode <= 2; ++mode) {
        PathNode* path = findGamePath(grid, 3, 4, 1, 0, 1, 3, mode);
        const PathNode* node = path;
        bool ok = true;
        std::cout << "Mode " << mode << '\n';

        for (int i = 0; i < 4; ++i) {
            const double h = heuristics[mode - 1][i];
            if (!node || node->name != labels[i] || node->g != costs[i] ||
                node->h != h || node->f != costs[i] + h) {
                ok = false;
                break;
            }

            std::cout << node->name << " | g=" << node->g << " h=" << node->h
                      << " f=" << node->f << '\n';
            node = node->next;
        }

        ok = ok && node == nullptr;
        freePath(path);
        if (!ok) return 1;
    }

    PathNode* same = findGamePath(grid, 3, 4, 1, 0, 1, 0, 2);
    const bool sameOk = same && same->name == "Start" && same->g == 0 &&
                        same->h == 0 && same->f == 0 && !same->next;
    freePath(same);
    if (!sameOk) return 1;

    grid[0][1] = grid[1][0] = 1;
    PathNode* blocked = findGamePath(grid, 2, 2, 0, 0, 1, 1, 2);
    const bool noPath = blocked == nullptr;
    freePath(blocked);
    if (!noPath) return 1;

    std::cout << "PASS\n";
    return 0;
}

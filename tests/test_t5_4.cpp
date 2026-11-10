#include "Algo.h"

#include <iostream>

int main() {
    static int grid[100][100]{};
    static double weights[100][100]{};
    const char* labels[] = {"(0,0)", "(0,1)", "(0,2)",
                            "(1,0)", "(1,1)", "(1,2)"};

    for (int mode = 1; mode <= 2; ++mode) {
        PathNode* path = findPCBPath(grid, 2, 3, 0, 0, 1, 2, weights, mode);
        int previous = -1;
        int lastDirection = 0;
        int steps = 0;
        int turns = 0;
        double length = 0.0;
        bool ok = path && path->name == labels[0];

        std::cout << "Mode " << mode;
        for (const PathNode* node = path; node && ok; node = node->next) {
            int cell = 0;
            while (cell < 6 && node->name != labels[cell]) ++cell;
            if (cell == 6) {
                ok = false;
                break;
            }

            if (previous >= 0) {
                const double weight = weights[previous][cell];
                const int direction = cell - previous;
                ok = weight == 1.0;
                length += weight;
                if (steps > 0 && direction != lastDirection) ++turns;
                lastDirection = direction;
                ++steps;
            }

            const int dx = 1 - cell / 3;
            const int dy = 2 - cell % 3;
            const double h = mode == 1 ? dx + dy : (dx > dy ? dx : dy);
            ok = ok && node->g == length + 0.5 * turns &&
                 node->h == h && node->f == node->g + h;
            previous = cell;
            std::cout << " | " << node->name;
        }
        std::cout << " | g=" << length + 0.5 * turns << '\n';
        freePath(path);
        if (!ok || previous != 5 || steps != 3 || length != 3.0 || turns != 1) {
            return 1;
        }
    }

    grid[0][1] = grid[1][0] = 1;
    PathNode* blocked = findPCBPath(grid, 2, 3, 0, 0, 1, 2, weights, 1);
    const bool noPath = !blocked && weights[0][1] == 0.0 && weights[0][3] == 0.0;
    freePath(blocked);
    if (!noPath) return 1;

    PathNode* same = findPCBPath(grid, 2, 3, 0, 0, 0, 0, weights, 2);
    const bool sameOk = same && same->name == labels[0] && !same->next &&
                        same->g == 0.0 && same->h == 0.0 && same->f == 0.0;
    freePath(same);
    if (!sameOk) return 1;

    std::cout << "PASS\n";
    return 0;
}

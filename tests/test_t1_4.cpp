#include "Algo.h"

#include <cmath>
#include <iostream>

int main() {
    static double w[100][100]{};
    w[0][1] = 0.8;
    w[1][3] = 1.2;
    w[0][2] = 1.15;
    w[2][3] = 1.0;
    w[1][4] = w[1][5] = 9.0;

    PathNode* path = findMeshPath(w, 0, 3);
    const char* names[] = {"0", "2", "3"};
    const double costs[] = {0.0, 1.25, 2.25};
    const double estimates[] = {1.0, 1.0, 0.0};
    const PathNode* node = path;
    bool ok = true;

    for (int i = 0; i < 3; ++i) {
        if (node == nullptr) {
            ok = false;
            break;
        }

        std::cout << node->name << " g=" << node->g
                  << " h=" << node->h << " f=" << node->f << "\n";
        ok = ok && node->name == names[i] &&
             std::abs(node->g - costs[i]) < 1e-9 &&
             std::abs(node->h - estimates[i]) < 1e-9 &&
             std::abs(node->f - (costs[i] + estimates[i])) < 1e-9;
        node = node->next;
    }

    ok = ok && node == nullptr;
    freePath(path);
    std::cout << (ok ? "PASS" : "FAIL") << "\n";
    return ok ? 0 : 1;
}

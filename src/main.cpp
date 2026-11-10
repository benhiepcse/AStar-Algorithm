#include "Algo.h"

#include <iostream>

namespace {

bool showPath(const char* title, PathNode* head) {
    std::cout << "\n" << title << "\n";
    const bool found = head != nullptr;
    if (!found) {
        std::cout << "No path\n";
    }
    for (const PathNode* node = head; node != nullptr; node = node->next) {
        std::cout << node->name << " | g=" << node->g
                  << " | h=" << node->h << " | f=" << node->f << "\n";
    }
    freePath(head);
    return found;
}

} // namespace

int main() {
    static double mesh[100][100]{};
    mesh[0][1] = mesh[1][0] = 2.0;
    const bool meshFound = showPath("Mesh", findMeshPath(mesh, 0, 1));

    static double energy[100][100]{};
    static int xyz[100][3]{};
    energy[0][1] = energy[1][0] = 10.0;
    xyz[1][2] = 100;
    const bool auvFound = showPath("AUV", findAUVPath(energy, xyz, 0, 1, 2));

    static double distance[100][100]{};
    static double risk[100][100]{};
    static int xy[100][2]{};
    distance[0][1] = distance[1][0] = 5.0;
    risk[0][1] = risk[1][0] = 0.25;
    xy[1][0] = 1;
    PathNode* hazard = findProbabilisticPath(distance, risk, xy, 0, 1, 4.0, 1);
    const bool hazardFound = showPath("Probabilistic", hazard);
    if (hazardFound) {
        // This fixture has exactly one possible route: 0 -> 1.
        std::cout << "Sample P_safe=" << (1.0 - risk[0][1]) << "\n";
    }

    static int game[100][100]{};
    game[0][1] = 2;
    const bool gameFound = showPath("Game", findGamePath(game, 1, 2, 0, 0, 0, 1, 1));

    static int pcb[100][100]{};
    static double weights[100][100]{};
    const bool pcbFound = showPath("PCB", findPCBPath(pcb, 1, 2, 0, 0, 0, 1, weights, 1));
    std::cout << "PCB weight matrix (2 x 2):\n";
    for (int row = 0; row < 2; ++row) {
        std::cout << weights[row][0] << " " << weights[row][1] << "\n";
    }

    return meshFound && auvFound && hazardFound && gameFound && pcbFound ? 0 : 1;
}

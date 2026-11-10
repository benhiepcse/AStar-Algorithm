#include "Algo.cpp"

#include <iostream>

int main() {
    static int grid[100][100]{};
    static double weights[100][100]{};
    grid[0][1] = 1;
    const int expected[6][6] = {
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0, 1},
        {0, 0, 1, 0, 1, 0},
    };

    for (auto& row : weights) {
        for (double& value : row) value = 9.0;
    }
    if (!buildPCBWeightMatrix(grid, 2, 3, weights)) return 1;

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            const double want = i < 6 && j < 6 ? expected[i][j] : 0.0;
            if (weights[i][j] != want) return 1;
            if (i < 6 && j < 6) {
                std::cout << weights[i][j] << (j == 5 ? '\n' : ' ');
            }
        }
    }

    if (!buildPCBWeightMatrix(grid, 10, 10, weights) ||
        weights[98][99] != 1.0) return 1;
    if (buildPCBWeightMatrix(grid, 11, 10, weights) ||
        buildPCBWeightMatrix(grid, 0, 10, weights) ||
        weights[98][99] != 1.0) return 1;

    std::cout << "PASS\n";
    return 0;
}

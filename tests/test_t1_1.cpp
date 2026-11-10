#include "Algo.cpp"

#include <iostream>

int main() {
    static double matrix[100][100]{};
    matrix[0][0] = 9.0;
    matrix[0][1] = 2.0;
    matrix[0][2] = 1e9;
    matrix[1][2] = 3.0;
    matrix[2][0] = -1.0;

    std::vector<int> expected(100, 0);
    expected[0] = 2;
    expected[1] = 1;
    const auto degrees = computeMeshDegrees(matrix);

    for (int id = 0; id < 4; ++id) {
        std::cout << id << ": " << degrees[id] << "\n";
    }
    return degrees == expected ? 0 : 1;
}

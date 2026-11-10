#include "Algo.cpp"

#include <iostream>
#include <string>
#include <vector>

bool checkSteps(const char* title, const int grid[100][100], int x, int y,
                const std::vector<std::string>& expected) {
    std::vector<std::string> actual;
    forEachGameStep(grid, 3, 4, x, y,
        [&actual](int nextX, int nextY, const GameDirection& direction) {
            actual.push_back(std::string(direction.label) + "(" +
                std::to_string(nextX) + "," + std::to_string(nextY) + ")");
        });

    std::cout << title;
    for (const auto& step : actual) {
        std::cout << " | " << step;
    }
    std::cout << '\n';
    return actual == expected;
}

int main() {
    static int grid[100][100]{};
    if (!checkSteps("Corner", grid, 0, 0,
                    {"Down(1,0)", "Right(0,1)", "Down-Right(1,1)"}) ||
        !checkSteps("Edge", grid, 1, 3,
                    {"Up(0,3)", "Down(2,3)", "Left(1,2)",
                     "Up-Left(0,2)", "Down-Left(2,2)"}) ||
        !checkSteps("Interior", grid, 1, 1,
                    {"Up(0,1)", "Down(2,1)", "Left(1,0)", "Right(1,2)",
                     "Up-Left(0,0)", "Up-Right(0,2)", "Down-Left(2,0)",
                     "Down-Right(2,2)"})) {
        return 1;
    }

    grid[0][1] = 1;
    grid[1][2] = 2;
    if (!checkSteps("Obstacle and swamp", grid, 1, 1,
                {"Down(2,1)", "Left(1,0)", "Right(1,2)",
                 "Down-Left(2,0)", "Down-Right(2,2)"})) {
    return 1;
    }

    std::cout << "PASS\n";
    return 0;
}

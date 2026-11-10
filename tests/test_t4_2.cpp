#include "Algo.cpp"

#include <iostream>

int main() {
    static int grid[100][100]{};
    const struct {
        int horizontal;
        int vertical;
        int destination;
        bool allowed;
    } cases[] = {
        {1, 0, 0, false},
        {0, 1, 0, false},
        {0, 0, 0, true},
        {2, 0, 0, true},
        {0, 0, 1, false},
    };

    for (const auto& test : cases) {
        grid[0][1] = test.horizontal;
        grid[1][0] = test.vertical;
        grid[1][1] = test.destination;

        bool found = false;
        forEachGameStep(grid, 2, 2, 0, 0,
            [&found](int nextX, int nextY, const GameDirection&) {
                if (nextX == 1 && nextY == 1) {
                    found = true;
                }
            });

        std::cout << "H=" << test.horizontal << " V=" << test.vertical
                  << " D=" << test.destination << " | Down-Right="
                  << (found ? "allowed" : "blocked") << '\n';
        if (found != test.allowed) return 1;
    }

    std::cout << "PASS\n";
    return 0;
}

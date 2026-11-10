#include "Algo.cpp"

#include <iostream>

struct CountedAuvNeighbors {
    AuvNeighbors neighbors;
    int& expansions;

    template <typename Emit>
    void operator()(int from, Emit emit) const {
        ++expansions;
        neighbors(from, emit);
    }
};

bool samePath(const PathNode* a, const PathNode* b) {
    while (a != nullptr && b != nullptr) {
        if (a->name != b->name || a->g != b->g ||
            a->h != b->h || a->f != b->f) {
            return false;
        }
        a = a->next;
        b = b->next;
    }
    return a == nullptr && b == nullptr;
}

bool runCase(const char* title, double weights[100][100],
             int coords[100][3], int goal,
             const double expectedCosts[4], const int expectedExpansions[4]) {
    std::cout << title << '\n';
    for (int mode = 0; mode <= 3; ++mode) {
        const auto heuristic = [coords, goal, mode](int id) {
            return mode == 0 ? 0.0 : auvHeuristic(coords[id], coords[goal], mode);
        };
        int expansions = 0;
        SearchState search(AuvStationCount, 0, heuristic);
        const int reached = searchUntilGoal(
            search, heuristic, CountedAuvNeighbors{{weights, coords}, expansions},
            [goal](int id) { return id == goal; });
        PathNode* path = reconstructPath(search, reached, [](int id, int) {
            return std::to_string(id);
        });

        bool ok = path != nullptr && reached == goal &&
                  expansions == expectedExpansions[mode];
        if (mode != 0) {
            PathNode* apiPath = findAUVPath(weights, coords, 0, goal, mode);
            ok = ok && samePath(path, apiPath);
            freePath(apiPath);
        }

        std::cout << "Mode " << mode << " | ";
        double cost = 0.0;
        for (const PathNode* node = path; node != nullptr; node = node->next) {
            std::cout << node->name;
            if (node->next != nullptr) std::cout << " -> ";
            cost = node->g;
        }
        std::cout << " | g=" << cost << " | expanded=" << expansions << '\n';
        freePath(path);
        if (!ok || cost != expectedCosts[mode]) return false;
    }
    return true;
}

int main() {
    static double a[100][100]{};
    static int xyzA[100][3]{};
    xyzA[1][0] = 5;
    xyzA[2][0] = 10;
    xyzA[3][0] = -5;
    a[0][1] = 5;
    a[1][2] = 5;
    a[0][2] = 12;
    a[0][3] = 5;
    const double costsA[] = {10, 10, 10, 10};
    const int expansionsA[] = {3, 2, 2, 2};

    static double b[100][100]{};
    static int xyzB[100][3]{};
    xyzB[1][0] = 100;
    b[0][1] = 1;
    b[1][2] = 1;
    b[0][2] = 5;
    const double costsB[] = {2, 5, 5, 5};
    const int expansionsB[] = {2, 1, 1, 1};

    if (!runCase("A: Same optimal cost", a, xyzA, 2, costsA, expansionsA) ||
        !runCase("B: Heuristic overestimates", b, xyzB, 2, costsB, expansionsB)) {
        return 1;
    }
    std::cout << "PASS\n";
    return 0;
}

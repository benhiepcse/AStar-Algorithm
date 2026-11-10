#include "Algo.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using Matrix = std::vector<std::vector<double>>;

constexpr double Unreachable = 1e30;

double reference(const Matrix& edges, int goal) {
    const int n = static_cast<int>(edges.size());
    std::vector<double> costs(n, Unreachable);
    std::vector<bool> visited(n, false);
    costs[0] = 0.0;

    for (int round = 0; round < n; ++round) {
        int from = -1;
        for (int id = 0; id < n; ++id) {
            if (!visited[id] && (from < 0 || costs[id] < costs[from])) {
                from = id;
            }
        }
        if (from == goal || costs[from] == Unreachable) {
            return costs[goal];
        }

        visited[from] = true;
        for (int to = 0; to < n; ++to) {
            const double weight = edges[from][to];
            if (weight > 0.0 && costs[from] + weight < costs[to]) {
                costs[to] = costs[from] + weight;
            }
        }
    }
    return costs[goal];
}

bool check(PathNode* path, const Matrix& edges, int goal, double expected) {
    const int n = static_cast<int>(edges.size());
    bool ok = expected == Unreachable ? path == nullptr : path != nullptr;
    int previous = -1;
    int count = 0;
    double cost = 0.0;

    for (const PathNode* node = path; node && ok; node = node->next) {
        int id = 0;
        while (id < n && node->name != std::to_string(id)) {
            ++id;
        }
        if (id == n || ++count > n ||
            (previous < 0 ? id != 0 : edges[previous][id] <= 0.0)) {
            ok = false;
            break;
        }

        if (previous >= 0) {
            cost += edges[previous][id];
        }
        ok = std::abs(node->g - cost) < 1e-9 &&
             std::abs(node->f - (node->g + node->h)) < 1e-9;
        previous = id;
    }

    if (path) {
        ok = ok && previous == goal && std::abs(cost - expected) < 1e-9;
    }
    freePath(path);
    return ok;
}

int main() {
    static double weights[100][100]{}, risk[100][100]{};
    static int xyz[100][3]{}, xy[100][2]{};
    const int n = 7;
    const int goal = n - 1;
    unsigned seed = 1007;

    for (int sample = 0; sample < 64; ++sample) {
        Matrix mesh(n, std::vector<double>(n));
        Matrix auv = mesh;
        Matrix hazard = mesh;

        for (int u = 0; u < n; ++u) {
            xyz[u][2] = u;
            for (int v = 0; v < n; ++v) {
                seed = seed * 1664525u + 1013904223u;
                weights[u][v] = u != v && (seed >> 16) % 4 == 0
                    ? 8 + (seed >> 20) % 13 : 0;
                risk[u][v] = ((seed >> 25) % 4) * 0.125;
            }
        }

        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                const double weight = weights[u][v];
                if (weight == 0.0) continue;

                int degree = 0;
                for (int k = 0; k < n; ++k) {
                    degree += weights[v][k] > 0.0;
                }

                mesh[u][v] = weight + 0.1 * degree;
                auv[u][v] = weight * (1.0 + std::abs(v - u) / 100.0);
                hazard[u][v] = weight + 4.0 * risk[u][v];
            }
        }

        bool ok = check(findMeshPath(weights, 0, goal),
                        mesh, goal, reference(mesh, goal));
        for (int mode = 1; mode <= 3; ++mode) {
            ok = check(findAUVPath(weights, xyz, 0, goal, mode),
                       auv, goal, reference(auv, goal)) && ok;
        }
        for (int mode = 1; mode <= 2; ++mode) {
            ok = check(findProbabilisticPath(weights, risk, xy, 0, goal, 4, mode),
                       hazard, goal, reference(hazard, goal)) && ok;
        }

        if (!ok) {
            std::cerr << "FAIL: seed=1007 sample=" << sample << '\n';
            return 1;
        }
    }

    std::cout << "PASS: 384 graph oracle cases\n";
    return 0;
}

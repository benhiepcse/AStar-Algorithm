#include "Algo.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr int MeshNodeCount = 100;

bool isMeshEdge(const double weightMatrix[][MeshNodeCount], int from, int to) {
    return from != to && weightMatrix[from][to] > 0.0;
}

inline std::vector<int> computeMeshDegrees(
    const double weightMatrix[][MeshNodeCount]) {
    std::vector<int> degrees(MeshNodeCount, 0);
    for (int from = 0; from < MeshNodeCount; ++from) {
        for (int to = 0; to < MeshNodeCount; ++to) {
            if (isMeshEdge(weightMatrix, from, to)) {
                ++degrees[from];
            }
        }
    }
    return degrees;
}

inline double meshEdgeCost(const double weightMatrix[][MeshNodeCount],
                           const std::vector<int>& degrees, int from, int to) {
    return weightMatrix[from][to] + 0.1 * degrees[to];
}

inline double meshGoalLowerBound(const double weightMatrix[][MeshNodeCount],
                                 const std::vector<int>& degrees, int goal) {
    double best = 0.0;
    for (int from = 0; from < MeshNodeCount; ++from) {
        if (isMeshEdge(weightMatrix, from, goal)) {
            const double cost = meshEdgeCost(weightMatrix, degrees, from, goal);
            if (best == 0.0 || cost < best) {
                best = cost;
            }
        }
    }
    return best;
}

inline double auvEdgeCost(double weight, int fromDepth, int toDepth) {
    const double depthDifference =
        std::abs(static_cast<double>(toDepth) - static_cast<double>(fromDepth));
    return weight * (1.0 + depthDifference / 100.0);
}

inline double auvHeuristic(const int from[3], const int goal[3], int mode) {
    const double dx = std::abs(static_cast<double>(from[0]) - goal[0]);
    const double dy = std::abs(static_cast<double>(from[1]) - goal[1]);
    const double dz = std::abs(static_cast<double>(from[2]) - goal[2]);

    switch (mode) {
        case 1:
            return dx + dy + dz;
        case 2:
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        case 3:
            return std::max(dx, std::max(dy, dz));
        default:
            return 0.0;
    }
}

struct SearchRecord {
    double g = 0.0;
    double h = 0.0;
    double incomingCost = 0.0;
    int parent = -1;
    bool reached = false;
};

struct FrontierEntry {
    int stateId;
    double f;
    double h;
    double g;
};

struct LowerPriority {
    bool operator()(const FrontierEntry& a, const FrontierEntry& b) const {
        if (a.f != b.f) return a.f > b.f;
        if (a.h != b.h) return a.h > b.h;
        return a.stateId > b.stateId;
    }
};

struct SearchState {
    std::vector<SearchRecord> records;
    std::vector<FrontierEntry> frontier;

    // The caller supplies a positive state count and a valid start ID.
    template <typename Heuristic>
    SearchState(int stateCount, int startId, Heuristic heuristic)
        : records(stateCount) {
        SearchRecord& start = records[startId];
        start.h = heuristic(startId);
        start.reached = true;
        frontier.push_back({startId, start.g + start.h, start.h, start.g});
    }
};

// Neighbors must emit valid IDs and finite, nonnegative costs synchronously.
// The heuristic must be deterministic and all accumulated scores must be finite.
template <typename Heuristic, typename Neighbors, typename Goal>
int searchUntilGoal(SearchState& search, Heuristic heuristic,
                    Neighbors neighbors, Goal isGoal) {
    while (!search.frontier.empty()) {
        std::pop_heap(search.frontier.begin(), search.frontier.end(), LowerPriority{});
        const FrontierEntry current = search.frontier.back();
        search.frontier.pop_back();

        if (current.g != search.records[current.stateId].g) {
            continue;
        }
        if (isGoal(current.stateId)) {
            return current.stateId;
        }

        neighbors(current.stateId, [&](int nextId, double edgeCost) {
            SearchRecord& next = search.records[nextId];
            const double candidateG = current.g + edgeCost;
            if (next.reached && candidateG >= next.g) {
                return;
            }

            const double h = next.reached ? next.h : heuristic(nextId);
            search.frontier.push_back({nextId, candidateG + h, h, candidateG});
            std::push_heap(search.frontier.begin(), search.frontier.end(), LowerPriority{});
            next.g = candidateG;
            next.h = h;
            next.parent = current.stateId;
            next.incomingCost = edgeCost;
            next.reached = true;
        });
    }
    return -1;
}

// Name receives the full state ID and its parent ID (-1 at start).
template <typename Name>
PathNode* reconstructPath(const SearchState& search, int goalId, Name name) {
    if (goalId < 0) {
        return nullptr;
    }

    std::vector<int> route;
    for (int id = goalId; id != -1; id = search.records[id].parent) {
        route.push_back(id);
    }

    std::vector<PathStep> steps;
    steps.reserve(route.size());
    double cost = 0.0;
    for (auto id = route.rbegin(); id != route.rend(); ++id) {
        const SearchRecord& record = search.records[*id];
        cost += record.incomingCost;
        const double h = route.size() == 1 ? 0.0 : record.h;
        steps.push_back({name(*id, record.parent), cost, h});
    }
    return buildPathList(steps);
}

struct PCBStateKey {
    enum { None = 4, SlotsPerCell = 5 };

    static int encode(int cellId, int incomingDirection) {
        return cellId * SlotsPerCell + incomingDirection;
    }

    static int cellOf(int stateId) {
        return stateId / SlotsPerCell;
    }

    static int directionOf(int stateId) {
        return stateId % SlotsPerCell;
    }
};

struct MeshNeighbors {
    const double (*weights)[MeshNodeCount];
    const std::vector<int>& degrees;

    template <typename Emit>
    void operator()(int from, Emit emit) const {
        for (int to = 0; to < MeshNodeCount; ++to) {
            if (isMeshEdge(weights, from, to)) {
                emit(to, meshEdgeCost(weights, degrees, from, to));
            }
        }
    }
};

}  // namespace

PathNode* findMeshPath(double weightMatrix[100][100],
                       int startNode, int goalNode) {
    if (weightMatrix == nullptr ||
        startNode < 0 || startNode >= MeshNodeCount ||
        goalNode < 0 || goalNode >= MeshNodeCount) {
        return nullptr;
    }

    const auto degrees = computeMeshDegrees(weightMatrix);
    const double bound = meshGoalLowerBound(weightMatrix, degrees, goalNode);
    const auto heuristic = [goalNode, bound](int id) {
        return id == goalNode ? 0.0 : bound;
    };

    SearchState search(MeshNodeCount, startNode, heuristic);
    const int goal = searchUntilGoal(
        search, heuristic, MeshNeighbors{weightMatrix, degrees},
        [goalNode](int id) { return id == goalNode; });

    return reconstructPath(search, goal, [](int id, int) {
        return std::to_string(id);
    });
}

namespace {

constexpr int AuvStationCount = 100;

struct AuvNeighbors {
    const double (*weights)[AuvStationCount];
    const int (*coords)[3];

    template <typename Emit>
    void operator()(int from, Emit emit) const {
        for (int to = 0; to < AuvStationCount; ++to) {
            const double weight = weights[from][to];
            if (from != to && weight > 0.0 && std::isfinite(weight)) {
                const double cost =
                    auvEdgeCost(weight, coords[from][2], coords[to][2]);
                if (std::isfinite(cost)) {
                    emit(to, cost);
                }
            }
        }
    }
};

}  // namespace

PathNode* findAUVPath(double weightMatrix[100][100], int coords[100][3],
                      int startPoint, int goalPoint, int mode) {
    if (weightMatrix == nullptr || coords == nullptr ||
        startPoint < 0 || startPoint >= AuvStationCount ||
        goalPoint < 0 || goalPoint >= AuvStationCount ||
        mode < 1 || mode > 3) {
        return nullptr;
    }

    const auto heuristic = [coords, goalPoint, mode](int id) {
        return auvHeuristic(coords[id], coords[goalPoint], mode);
    };

    SearchState search(AuvStationCount, startPoint, heuristic);
    const int goal = searchUntilGoal(
        search, heuristic, AuvNeighbors{weightMatrix, coords},
        [goalPoint](int id) { return id == goalPoint; });

    return reconstructPath(search, goal, [](int id, int) {
        return std::to_string(id);
    });
}

namespace {

inline double probabilisticEdgeCost(double distance, double risk, double lambda) {
    return distance + lambda * risk;
}

inline double probabilisticHeuristic(const int from[2], const int goal[2],
                                     int mode) {
    const double dx = std::abs(static_cast<double>(from[0]) - goal[0]);
    const double dy = std::abs(static_cast<double>(from[1]) - goal[1]);

    switch (mode) {
        case 1:
            return dx + dy;
        case 2:
            return std::sqrt(dx * dx + dy * dy);
        default:
            return 0.0;
    }
}

}  // namespace

namespace {

inline double probabilisticPathSafety(const double riskMatrix[100][100],
                                      const std::vector<int>& route) {
    double safety = 1.0;
    for (auto node = route.begin(); node != route.end(); ++node) {
        const auto next = node + 1;
        if (next == route.end()) {
            break;
        }
        safety *= 1.0 - riskMatrix[*node][*next];
    }
    return safety;
}

}  // namespace

namespace {

constexpr int ProbabilisticNodeCount = 100;
thread_local double lastProbabilisticSafety = -1.0;

struct ProbabilisticNeighbors {
    const double (*distances)[ProbabilisticNodeCount];
    const double (*risks)[ProbabilisticNodeCount];
    double lambda;

    template <typename Emit>
    void operator()(int from, Emit emit) const {
        for (int to = 0; to < ProbabilisticNodeCount; ++to) {
            const double distance = distances[from][to];
            const double risk = risks[from][to];
            if (from == to || distance <= 0.0 || !std::isfinite(distance) ||
                !(risk >= 0.0 && risk < 1.0)) {
                continue;
            }
            const double cost = probabilisticEdgeCost(distance, risk, lambda);
            if (std::isfinite(cost)) {
                emit(to, cost);
            }
        }
    }
};

}  // namespace

double getLastProbabilisticPathSafety() {
    return lastProbabilisticSafety;
}

PathNode* findProbabilisticPath(double distMatrix[100][100],
                                double riskMatrix[100][100],
                                int coords[100][2], int startPoint,
                                int goalPoint, double lambda, int mode) {
    lastProbabilisticSafety = -1.0;
    if (distMatrix == nullptr || riskMatrix == nullptr || coords == nullptr ||
        startPoint < 0 || startPoint >= ProbabilisticNodeCount ||
        goalPoint < 0 || goalPoint >= ProbabilisticNodeCount ||
        !std::isfinite(lambda) || lambda < 0.0 || mode < 1 || mode > 2) {
        return nullptr;
    }

    const auto heuristic = [coords, goalPoint, mode](int id) {
        return probabilisticHeuristic(coords[id], coords[goalPoint], mode);
    };
    SearchState search(ProbabilisticNodeCount, startPoint, heuristic);
    const int goal = searchUntilGoal(
        search, heuristic, ProbabilisticNeighbors{distMatrix, riskMatrix, lambda},
        [goalPoint](int id) { return id == goalPoint; });

    std::vector<int> route;
    PathNode* path = reconstructPath(search, goal, [&route](int id, int) {
        route.push_back(id);
        return std::to_string(id);
    });
    if (path != nullptr) {
        lastProbabilisticSafety = probabilisticPathSafety(riskMatrix, route);
    }
    return path;
}

namespace {

struct GameDirection {
    int dx;
    int dy;
    const char* label;
};

constexpr GameDirection GameDirections[] = {
    {-1,  0, "Up"},
    { 1,  0, "Down"},
    { 0, -1, "Left"},
    { 0,  1, "Right"},
    {-1, -1, "Up-Left"},
    {-1,  1, "Up-Right"},
    { 1, -1, "Down-Left"},
    { 1,  1, "Down-Right"},
};

template <typename Emit>
void forEachGameStep(const int grid[100][100], int rows, int cols,
                     int x, int y, Emit emit) {
    for (const auto& direction : GameDirections) {
        const int nextX = x + direction.dx;
        const int nextY = y + direction.dy;
        if (nextX >= 0 && nextX < rows && nextY >= 0 && nextY < cols &&
            grid[nextX][nextY] != 1) {
            const bool diagonal = direction.dx != 0 && direction.dy != 0;
            if (diagonal && (grid[nextX][y] == 1 || grid[x][nextY] == 1)) {
                continue;
            }
            emit(nextX, nextY, direction);
        }
    }
}

}  // namespace

namespace {

inline double gameStepCost(int destinationTerrain, const GameDirection& direction) {
    const double base = destinationTerrain == 2 ? 2.0 : 1.0;
    const bool diagonal = direction.dx != 0 && direction.dy != 0;
    return base * (diagonal ? 1.5 : 1.0);
}

inline double gameHeuristic(int x, int y, int goalX, int goalY, int mode) {
    const int dx = std::abs(x - goalX);
    const int dy = std::abs(y - goalY);
    if (mode == 1) return dx + dy;
    if (mode == 2) return std::max(dx, dy);
    return 0.0;
}

}  // namespace

namespace {

struct GameNeighbors {
    const int (*grid)[100];
    int rows;
    int cols;

    template <typename Emit>
    void operator()(int id, Emit emit) const {
        forEachGameStep(grid, rows, cols, id / cols, id % cols,
            [&](int x, int y, const GameDirection& direction) {
                emit(x * cols + y, gameStepCost(grid[x][y], direction));
            });
    }
};

}  // namespace

PathNode* findGamePath(int mapGrid[100][100], int m, int n, int startX,
                       int startY, int goalX, int goalY, int mode) {
    if (mapGrid == nullptr || m < 1 || m > 100 || n < 1 || n > 100 ||
        startX < 0 || startX >= m || startY < 0 || startY >= n ||
        goalX < 0 || goalX >= m || goalY < 0 || goalY >= n ||
        (mode != 1 && mode != 2)) {
        return nullptr;
    }
    if (mapGrid[startX][startY] == 1 || mapGrid[goalX][goalY] == 1) {
        return nullptr;
    }

    const int startId = startX * n + startY;
    const int goalId = goalX * n + goalY;
    const auto heuristic = [n, goalX, goalY, mode](int id) {
        return gameHeuristic(id / n, id % n, goalX, goalY, mode);
    };

    SearchState search(m * n, startId, heuristic);
    const int reached = searchUntilGoal(
        search, heuristic, GameNeighbors{mapGrid, m, n},
        [goalId](int id) { return id == goalId; });

    return reconstructPath(search, reached, [n](int id, int parent) {
        if (parent < 0) return std::string("Start");
        const int dx = id / n - parent / n;
        const int dy = id % n - parent % n;
        for (const auto& direction : GameDirections) {
            if (direction.dx == dx && direction.dy == dy) {
                return std::string(direction.label);
            }
        }
        return std::string();
    });
}

namespace {

constexpr int PCBMatrixSize = 100;

inline bool buildPCBWeightMatrix(const int grid[100][100], int m, int n,
                                 double weights[100][100]) {
    if (!grid || !weights || m < 1 || n < 1 || m > PCBMatrixSize / n) {
        return false;
    }

    for (int id = 0; id < PCBMatrixSize; ++id) {
        std::fill(weights[id], weights[id] + PCBMatrixSize, 0.0);
    }

    for (int x = 0; x < m; ++x) {
        for (int y = 0; y < n; ++y) {
            if (grid[x][y] != 0) continue;

            const int from = x * n + y;
            for (int k = 0; k < 4; ++k) {
                const int nx = x + GameDirections[k].dx;
                const int ny = y + GameDirections[k].dy;
                if (nx >= 0 && nx < m && ny >= 0 && ny < n &&
                    grid[nx][ny] == 0) {
                    weights[from][nx * n + ny] = 1.0;
                }
            }
        }
    }
    return true;
}

}  // namespace

namespace {

inline double pcbStepCost(double weight, int incomingDirection,
                          int outgoingDirection) {
    const bool turns = incomingDirection != PCBStateKey::None &&
                       incomingDirection != outgoingDirection;
    return weight + (turns ? 0.5 : 0.0);
}

inline double pcbHeuristic(int cellId, int goalCellId, int cols, int mode) {
    return gameHeuristic(cellId / cols, cellId % cols,
                         goalCellId / cols, goalCellId % cols, mode);
}

}  // namespace

namespace {

struct PCBNeighbors {
    const double (*weights)[PCBMatrixSize];
    int rows;
    int cols;

    template <typename Emit>
    void operator()(int state, Emit emit) const {
        const int cell = PCBStateKey::cellOf(state);
        const int incoming = PCBStateKey::directionOf(state);
        for (int dir = 0; dir < 4; ++dir) {
            const int x = cell / cols + GameDirections[dir].dx;
            const int y = cell % cols + GameDirections[dir].dy;
            if (x < 0 || x >= rows || y < 0 || y >= cols) continue;

            const int next = x * cols + y;
            const double weight = weights[cell][next];
            if (weight > 0.0) {
                emit(PCBStateKey::encode(next, dir),
                     pcbStepCost(weight, incoming, dir));
            }
        }
    }
};

}  // namespace

PathNode* findPCBPath(int pcbGrid[100][100], int m, int n, int startX,
                      int startY, int exitX, int exitY,
                      double weightMatrix[100][100], int mode) {
    if ((mode != 1 && mode != 2) ||
        !buildPCBWeightMatrix(pcbGrid, m, n, weightMatrix)) {
        return nullptr;
    }
    if (startX < 0 || startX >= m || startY < 0 || startY >= n ||
        exitX < 0 || exitX >= m || exitY < 0 || exitY >= n ||
        pcbGrid[startX][startY] != 0 || pcbGrid[exitX][exitY] != 0) {
        return nullptr;
    }

    const int exitCell = exitX * n + exitY;
    const int start = PCBStateKey::encode(startX * n + startY, PCBStateKey::None);
    const auto heuristic = [exitCell, n, mode](int state) {
        return pcbHeuristic(PCBStateKey::cellOf(state), exitCell, n, mode);
    };

    SearchState search(m * n * PCBStateKey::SlotsPerCell, start, heuristic);
    const int reached = searchUntilGoal(
        search, heuristic, PCBNeighbors{weightMatrix, m, n},
        [exitCell](int state) { return PCBStateKey::cellOf(state) == exitCell; });

    return reconstructPath(search, reached, [n](int state, int) {
        const int cell = PCBStateKey::cellOf(state);
        return "(" + std::to_string(cell / n) + "," +
               std::to_string(cell % n) + ")";
    });
}

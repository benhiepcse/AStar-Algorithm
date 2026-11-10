#include "Algo.h"

#include <algorithm>

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

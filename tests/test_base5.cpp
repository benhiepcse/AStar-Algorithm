#include "Algo.cpp"

#include <iostream>
#include <functional>

struct Edge {
    int to;
    double cost;
};

bool checkSearch() {
    const std::vector<std::vector<Edge>> graph{
        {{1, 5}, {2, 1}, {3, 20}, {4, 30}},
        {{3, 2}},
        {{1, 1}},
        {},
        {}
    };
    const auto h = [](int id) { return id == 2 ? 5.0 : 0.0; };
    SearchState search(5, 0, h);
    std::vector<int> expanded;
    const auto neighbors = [&](int id, const std::function<void(int, double)>& emit) {
        expanded.push_back(id);
        for (const Edge& edge : graph[id]) {
            emit(edge.to, edge.cost);
        }
    };
    const int result = searchUntilGoal(search, h, neighbors,
                                       [](int) { return false; });
    return result == -1 && expanded == std::vector<int>{0, 1, 2, 1, 3, 4} &&
           search.records[1].g == 2 && search.records[1].parent == 2 &&
           search.records[3].g == 4 && search.records[3].parent == 1;
}

bool checkTie() {
    const auto h = [](int id) { return id == 1 ? 2.0 : 1.0; };
    SearchState search(4, 0, h);
    const auto neighbors = [](int id, const std::function<void(int, double)>& emit) {
        if (id == 0) {
            emit(3, 2);
            emit(1, 1);
            emit(2, 2);
        }
    };
    return searchUntilGoal(search, h, neighbors,
                           [](int id) { return id != 0; }) == 2;
}

int main() {
    const bool search = checkSearch();
    const bool tie = checkTie();
    std::cout << "Relax, reopen, stale entries: " << (search ? "PASS" : "FAIL") << '\n'
              << "Tie-break: " << (tie ? "PASS" : "FAIL") << '\n';
    return search && tie ? 0 : 1;
}

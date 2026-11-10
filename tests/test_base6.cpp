#include "Algo.cpp"

#include <functional>
#include <iostream>

bool checkPath(int goal, const std::vector<PathStep>& expected) {
    const auto h = [](int id) { return id == 0 ? 3.0 : id == 1 ? 1.0 : 0.0; };
    SearchState search(4, 0, h);
    const auto neighbors = [](int id, const std::function<void(int, double)>& emit) {
        if (id == 0) {
            emit(2, 10);
            emit(1, 2);
        } else if (id == 1) {
            emit(2, 3);
        }
    };

    const int reached = searchUntilGoal(search, h, neighbors,
                                        [=](int id) { return id == goal; });
    PathNode* head = reconstructPath(search, reached, [](int id, int parent) {
        return std::to_string(parent) + "->" + std::to_string(id);
    });
    const PathNode* node = head;
    bool ok = reached == (expected.empty() ? -1 : goal);
    for (const PathStep& step : expected) {
        if (node == nullptr || node->name != step.name ||
            node->g != step.g || node->h != step.h ||
            node->f != step.g + step.h) {
            ok = false;
            break;
        }
        node = node->next;
    }
    ok = ok && node == nullptr;
    freePath(head);
    return ok;
}

int main() {
    const bool multiple = checkPath(2, {{"-1->0", 0, 3}, {"0->1", 2, 1}, {"1->2", 5, 0}});
    const bool single = checkPath(0, {{"-1->0", 0, 0}});
    const bool unreachable = checkPath(3, {});
    std::cout << "Multi-step path: " << (multiple ? "PASS" : "FAIL") << '\n'
              << "Start equals goal: " << (single ? "PASS" : "FAIL") << '\n'
              << "Unreachable goal: " << (unreachable ? "PASS" : "FAIL") << '\n';
    return multiple && single && unreachable ? 0 : 1;
}

#include "PathNode.h"

#include <iostream>

bool checkList(const std::vector<PathStep>& steps) {
    PathNode* head = buildPathList(steps);
    const PathNode* node = head;
    bool passed = true;

    for (const PathStep& step : steps) {
        if (node == nullptr || node->name != step.name ||
            node->g != step.g || node->h != step.h ||
            node->f != step.g + step.h) {
            passed = false;
            break;
        }
        node = node->next;
    }

    passed = passed && node == nullptr;
    freePath(head);
    return passed;
}

int main() {
    const bool empty = checkList({});
    const bool single = checkList({{"Start", 0, 0}});
    const bool multiple = checkList({
        {"Start", 0, 3}, {"Mid", 2, 1.5}, {"Goal", 4, 0}
    });

    std::cout << "Empty: " << (empty ? "PASS" : "FAIL") << '\n'
              << "Single: " << (single ? "PASS" : "FAIL") << '\n'
              << "Multiple: " << (multiple ? "PASS" : "FAIL") << '\n';
    return empty && single && multiple ? 0 : 1;
}

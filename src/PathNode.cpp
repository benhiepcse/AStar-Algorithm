#include "PathNode.h"

void freePath(PathNode* head) noexcept {
    while (head != nullptr) {
        PathNode* next = head->next;
        delete head;
        head = next;
    }
}

PathNode* buildPathList(const std::vector<PathStep>& steps) {
    PathNode* head = nullptr;

    try {
        for (auto step = steps.rbegin(); step != steps.rend(); ++step) {
            head = new PathNode{step->name, step->g + step->h,
                                step->g, step->h, head};
        }
    } catch (...) {
        freePath(head);
        throw;
    }

    return head;
}

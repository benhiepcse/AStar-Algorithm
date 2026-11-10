#include "Algo.cpp"

#include <iostream>

struct TestPCBEdges {
    int start, top, bottom, joinDown, joinRight, goal;

    template <typename Emit>
    void operator()(int id, Emit emit) const {
        if (id == start) {
            emit(top, 1.0);
            emit(bottom, 1.0);
        } else if (id == top) {
            emit(joinDown, 1.5);
        } else if (id == bottom) {
            emit(joinRight, 1.5);
        } else if (id == joinDown) {
            emit(goal, 1.5);
        } else if (id == joinRight) {
            emit(goal, 1.0);
        }
    }
};

int main() {
    using Key = PCBStateKey;
    const int down = 1;
    const int right = 3;
    const int start = Key::encode(0, Key::None);
    const int top = Key::encode(1, right);
    const int bottom = Key::encode(3, down);
    const int joinDown = Key::encode(4, down);
    const int joinRight = Key::encode(4, right);
    const int goal = Key::encode(5, right);
    const auto h = [](int) { return 0.0; };

    SearchState search(6 * Key::SlotsPerCell, start, h);
    const int reached = searchUntilGoal(
        search, h, TestPCBEdges{start, top, bottom, joinDown, joinRight, goal},
        [goal](int id) { return id == goal; });

    if (Key::cellOf(joinDown) != 4 || Key::cellOf(joinRight) != 4 ||
        Key::directionOf(joinDown) != down ||
        Key::directionOf(joinRight) != right ||
        !search.records[joinDown].reached || !search.records[joinRight].reached ||
        search.records[joinDown].g != 2.5 || search.records[joinRight].g != 2.5 ||
        search.records[joinDown].parent != top ||
        search.records[joinRight].parent != bottom ||
        reached != goal || search.records[goal].parent != joinRight) {
        return 1;
    }

    PathNode* path = reconstructPath(search, reached, [](int id, int) {
        return std::to_string(id);
    });
    const int expected[] = {start, bottom, joinRight, goal};
    const PathNode* node = path;
    bool ok = true;
    for (int id : expected) {
        if (!node || node->name != std::to_string(id)) {
            ok = false;
            break;
        }
        node = node->next;
    }

    ok = ok && node == nullptr && search.records[goal].g == 3.5;
    freePath(path);
    if (!ok) return 1;

    std::cout << "Cell 4: Down state=21, Right state=23\n"
              << "Goal costs: via Down=4, via Right=3.5\n"
              << "State path: 4 -> 16 -> 23 -> 28\n"
              << "PASS\n";
    return 0;
}

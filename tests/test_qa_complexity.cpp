#include "Algo.cpp"

#include <iostream>

struct Chain {
    int size;
    int& expanded;
    int& emitted;

    template <typename Emit>
    void operator()(int from, Emit emit) const {
        ++expanded;
        if (from + 1 < size) {
            ++emitted;
            emit(from + 1, 1.0);
        }
    }
};

int main() {
    const int sizes[] = {10, 25, 50, 100};
    for (int size : sizes) {
        int expanded = 0;
        int emitted = 0;
        const auto heuristic = [](int) { return 0.0; };

        SearchState search(size, 0, heuristic);
        const int goal = searchUntilGoal(
            search, heuristic, Chain{size, expanded, emitted},
            [size](int id) { return id == size - 1; });

        std::cout << "N=" << size << " expanded=" << expanded
                  << " emitted=" << emitted << '\n';

        if (goal != size - 1 || expanded != size - 1 || emitted != size - 1) {
            return 1;
        }
    }

    std::cout << "PASS: chain scaling\n";
    return 0;
}

#include "Algo.cpp"

#include <iostream>

int main() {
    const int startId = 2;
    int calls = 0;
    int evaluatedId = -1;
    const SearchState search(4, startId, [&](int id) {
        ++calls;
        evaluatedId = id;
        return 3.5;
    });

    const SearchRecord& start = search.records[startId];
    bool initial = search.records.size() == 4 &&
                   calls == 1 && evaluatedId == startId &&
                   start.reached && start.parent == -1 &&
                   start.g == 0.0 && start.h == 3.5 &&
                   search.frontier.size() == 1 &&
                   search.frontier.front().stateId == startId &&
                   search.frontier.front().f == 3.5;

    for (int id = 0; id < 4; ++id) {
        if (id != startId) {
            initial = initial && !search.records[id].reached &&
                      search.records[id].parent == -1;
        }
    }

    const int cell = 2;
    const int pcbStart = PCBStateKey::encode(cell, PCBStateKey::None);
    const SearchState pcb(3 * PCBStateKey::SlotsPerCell, pcbStart,
                          [](int) { return 0.0; });
    const bool pcbKey = PCBStateKey::encode(cell, 0) == 10 &&
                        PCBStateKey::encode(cell, 1) == 11 &&
                        pcbStart == 14 &&
                        pcb.frontier.size() == 1 &&
                        pcb.frontier.front().stateId == pcbStart &&
                        !pcb.records[10].reached && !pcb.records[11].reached;

    std::cout << "Initial state: " << (initial ? "PASS" : "FAIL") << '\n'
              << "PCB state keys: " << (pcbKey ? "PASS" : "FAIL") << '\n';
    return initial && pcbKey ? 0 : 1;
}

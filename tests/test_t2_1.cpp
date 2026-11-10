#include "Algo.cpp"

#include <cmath>
#include <iostream>

int main() {
    const struct {
        double weight;
        int fromDepth;
        int toDepth;
        double expected;
    } cases[] = {
        {10.0, 40, 40, 10.0},
        {10.0, 40, 140, 20.0},
        {10.0, 140, 40, 20.0},
        {8.0, 40, 65, 10.0},
        {1.0, -2000000000, 2000000000, 40000001.0},
    };

    for (const auto& test : cases) {
        const double actual = auvEdgeCost(test.weight, test.fromDepth, test.toDepth);
        std::cout << test.fromDepth << " -> " << test.toDepth
                  << " | actual=" << actual << " | expected=" << test.expected << '\n';
        if (!(std::abs(actual - test.expected) < 1e-9)) {
            return 1;
        }
    }

    std::cout << "PASS\n";
    return 0;
}

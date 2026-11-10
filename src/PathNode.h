#ifndef CO1007_PATH_NODE_H
#define CO1007_PATH_NODE_H

#include <string>
#include <vector>

struct PathNode {
    std::string name;
    double f;
    double g;
    double h;
    PathNode* next;
};

struct PathStep {
    std::string name;
    double g;
    double h;
};

PathNode* buildPathList(const std::vector<PathStep>& steps);
void freePath(PathNode* head) noexcept;

#endif

#ifndef CO1007_ALGO_H
#define CO1007_ALGO_H

#include "PathNode.h"

PathNode* findMeshPath(double weightMatrix[100][100],
                       int startNode, int goalNode);

PathNode* findAUVPath(double weightMatrix[100][100], int coords[100][3],
                      int startPoint, int goalPoint, int mode);

PathNode* findProbabilisticPath(double distMatrix[100][100],
                                double riskMatrix[100][100],
                                int coords[100][2], int startPoint,
                                int goalPoint, double lambda, int mode);

PathNode* findGamePath(int mapGrid[100][100], int m, int n,
                       int startX, int startY, int goalX, int goalY, int mode);

PathNode* findPCBPath(int pcbGrid[100][100], int m, int n,
                      int startX, int startY, int exitX, int exitY,
                      double weightMatrix[100][100], int mode);

double getLastProbabilisticPathSafety();

#endif

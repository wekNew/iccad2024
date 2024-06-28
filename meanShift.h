#pragma once
#ifndef MEANSHIFT_MEANSHIFT_H
#define MEANSHIFT_MEANSHIFT_H

#include <vector>

#include "Point.h"
#include "Cluster.h"
#include "cell.h"

#define MAX_ITERATIONS 100


void meanShift(std::vector<Cell>& cells, float bandwidth,int K, vector<vector<Cell*>> &clusters);
void buildClustersWithEpsilon(std::vector<Cell>& cells, float epsilon, vector<vector<Cell*>> &clusters);
int findRoot(int target, vector<int> &connected);
#endif //MEANSHIFT_MEANSHIFT_HPP
#pragma once
#ifndef MEANSHIFT_MEANSHIFT_H
#define MEANSHIFT_MEANSHIFT_H

#include <vector>

#include "Point.h"
#include "Cluster.h"
#include "cell.h"

#define MAX_ITERATIONS 100


void meanShift(std::vector<Cell>& cells, float bandwidth, int M,int K, float epsilon, vector<vector<Cell*>>& clusters);
float VariableBandwidth(float max_distance, float Mth_distance, Cell cell);
void buildClustersWithEpsilon(std::vector<Cell>& cells, float epsilon, vector<vector<Cell*>>& clusters);
int findRoot(int target, vector<int> &connected);
#endif 
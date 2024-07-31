#pragma once
#ifndef MEANSHIFT_MEANSHIFT_H
#define MEANSHIFT_MEANSHIFT_H

#include <vector>
#include<memory>
#include "Point.h"
#include "Cluster.h"
#include "cell.h"
using namespace std;
#define MAX_ITERATIONS 100


void meanShift(std::vector<shared_ptr<Cell>>& cells, float bandwidth, int M,int K, float epsilon, vector<vector<weak_ptr<Cell>>>& clusters);
float VariableBandwidth(float max_distance, float Mth_distance, shared_ptr<Cell> cell);
void buildClustersWithEpsilon(std::vector<shared_ptr<Cell>>& cells, float epsilon, vector<vector<weak_ptr<Cell>>>& clusters);
int findRoot(int target, vector<int> &connected);
#endif 
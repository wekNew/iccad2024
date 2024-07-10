#pragma once
#include <vector>
#include "cell.h"
#include "Point.h"
#include "table.h"
#ifndef PARTITION_H
#define PARTITION_H



void clusterToMBFF(vector<Cell*>& cells, Point& cluster_pos, vector<combination>& combi_table, vector<Cell> MBFF, int st_size, int rm_size);

int findFarthestPtoP(vector<Cell*>& cells, Point& p);

int findFarthestPointFromCentroid(vector<Cell*>& cells, vector<int>& clusterAssignment, int cluster, Point& centroid);

#endif 
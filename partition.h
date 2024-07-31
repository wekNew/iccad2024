#pragma once
#include <vector>
#include <string>
#include "cell.h"
#include "Point.h"
#include "table.h"
#ifndef PARTITION_H
#define PARTITION_H



void clusterToMBFF(vector<Cell*>& best_st_table, vector<shared_ptr<Cell>>& cells, Point& cluster_pos, vector<combination>& combi_table, vector<shared_ptr<Cell>>& MBFF, int st_size, int rm_size, int ff_num);

int findFarthestPtoP(vector<shared_ptr<Cell>>& cells, Point& p);

int findFarthestPointFromCentroid(vector<shared_ptr<Cell>>& cells, vector<int>& clusterAssignment, int cluster, Point& centroid);

#endif 
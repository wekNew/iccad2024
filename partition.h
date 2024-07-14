#pragma once
#include <vector>
#include <string>
#include "cell.h"
#include "Point.h"
#include "table.h"
#ifndef PARTITION_H
#define PARTITION_H



void clusterToMBFF(vector<Cell*>& best_st_table,vector<Cell*>& cells, Point& cluster_pos, vector<combination>& combi_table, vector<Cell>& MBFF, int st_size, int rm_size,int ff_num);

int findFarthestPtoP(vector<Cell*>& cells, Point& p);
double distance(Point& a, Point& b);
int findFarthestPointFromCentroid(vector<Cell*>& cells, vector<int>& clusterAssignment, int cluster, Point& centroid);

#endif 

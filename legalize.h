#pragma once
#ifndef LEGALIZE_H
#define LEGALIZE_H

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include"cell.h"
#include"Point.h"
#include"partition.h"

using namespace std;

// Define structures used in legalize.cpp
// Function prototypes

void to_the_site(int& x, int& y);
void print_overlap_graph(const unordered_map<int, unordered_set<int>>& overlap_graph);
void print_edges(const set<pair<int, int>>& edges);
void find_vertical_overlaps(vector<Cell>& clusters, unordered_map<int, unordered_set<int>>& overlap_graph);
void find_horizontal_overlaps(vector<Cell>& clusters, unordered_map<int, unordered_set<int>>& overlap_graph);
void creat_overlap_edge(unordered_map<int, unordered_set<int>>& graph, int direction);
//void merge_horizontal_vertical_overlap( set<pair<int, int>>& edges1,  set<pair<int, int>>& edges2, set<pair<int, int>>& overlap_edges, vector<pair<Cell, Cell>>& need_move_overlap_edges,set<int>& overlap_cluster,vector<Cell> clusters);
bool poriority_bigger(Cell& cluster1, Cell& cluster2);
bool check_overlap(const Cell& cluster1, const  Cell& cluster2);
vector<Cell> find_nearest_overlap_cluster(Cell& current_cluster, vector<Cell> overlap_clusters);
bool samewidth(Cell& cluster1, Cell& cluster2);
void move_cluster(Cell& current_cluster, const vector<Cell>& overlap_clusters, vector<Cell>& clusters);
queue<Cell> get_overlap_clusters(vector<Cell>& clusters, vector<pair<Cell, Cell>>& need_move_overlap_edges, vector<Cell>& overlap_clusters);
void shift_until_legal(vector<Cell> overlap_clusters, queue<Cell>& overlap_queue, vector<Cell>& clusters, vector<pair<Cell, Cell>>& need_move_overlap_edges);
void legalize(vector<Cell>& clusters, int bin_width, int bin_height, int die_x_min, int die_y_min, int die_x_max, int die_y_max);
#endif // LEGALIZE_H
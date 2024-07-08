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

using namespace std;

// Define structures used in legalize.cpp

struct cell {
    int bit;
    double slack;
    int width;
    int height;
};

struct cluster {
    int bit;
    double slack;
    double x_ordinate, y_ordinate;
    std::vector<cell> C;
    int cluster_name;
    int p_right;
    int p_left;
    int p_up;
    int p_down;
};



// Function prototypes

void to_the_site(double& x, double& y);
void print_overlap_graph(const unordered_map<int,unordered_set<int>>& overlap_graph);
void print_edges(const set<pair<int, int>>& edges);
void find_vertical_overlaps(vector<cluster>& clusters,unordered_map<int, unordered_set<int>>& overlap_graph);
void find_horizontal_overlaps(vector<cluster>& clusters, unordered_map<int, unordered_set<int>>& overlap_graph);
void creat_overlap_edge(unordered_map<int,unordered_set<int>>& graph, int direction);
void merge_horizontal_vertical_overlap(const set<pair<int, int>>& edges1, const set<pair<int, int>>& edges2, set<pair<int, int>>& overlap_edges, vector<pair<cluster, cluster>>& need_move_overlap_edges,set<int>& overlap_cluster,vector<cluster> clusters);
bool poriority_bigger(const cluster& cluster1, const cluster& cluster2);
bool check_overlap(const cluster& cluster1,const  cluster& cluster2);
vector<cluster> find_nearest_overlap_cluster(cluster& current_cluster,vector<cluster> overlap_clusters);
bool samewidth( cluster& cluster1, cluster& cluster2);
void move_cluster(cluster& current_cluster, const vector<cluster>& overlap_clusters, vector<cluster>& clusters);
queue<cluster> get_overlap_clusters(vector<cluster>& clusters,vector<pair<cluster, cluster>>& need_move_overlap_edges,vector<cluster>& overlap_clusters);
void shift_until_legal(vector<cluster> overlap_clusters, queue<cluster>& overlap_queue, vector<cluster>& clusters, vector<pair<cluster, cluster>>& need_move_overlap_edges);
void legalize(vector<cluster>& clusters);

#endif // LEGALIZE_H
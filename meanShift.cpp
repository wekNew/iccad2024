#include <vector>
#include <iostream>
#include <cmath>
#include <omp.h>
#include "Point.h"
#include "Cluster.h"
#include "ClustersBuilder.h"
#include "meanShift.h"
#include "Cell.h"
#include <algorithm>

void meanShift(std::vector<Cell>& cells, float bandwidth,int K, vector<vector<Cell*>> &clusters) {
    cout << "start to meanShift\n";
    std::vector<Point> points;
    for (const auto& cell : cells) {
        points.push_back(cell.getPos());
    }

    ClustersBuilder builder = ClustersBuilder(points, bandwidth);
    long iterations = 0;
    unsigned long dimensions = points[0].dimensions();
    float radius = bandwidth * 3;
    float doubledSquaredBandwidth = 2 * bandwidth * bandwidth;

    while (!builder.allPointsHaveStoppedShifting() && iterations < MAX_ITERATIONS) {
        cout << "\tcalculate shifted point\n";
#pragma omp parallel for default(none) \
            shared(points, dimensions, builder, bandwidth, radius, doubledSquaredBandwidth, cells, k) \
            schedule(dynamic)
        for (long i = 0; i < points.size(); ++i) {
            cout << "\t\tNo." << i << endl;
            if (builder.hasStoppedShifting(i))
                continue;

            Point newPosition(dimensions);
            Point pointToShift = builder.getShiftedPoint(i);
            float totalWeight = 0.0;

            // 找到最近的 k 个点
            std::vector<std::pair<float, Point>> neighbors;
            for (const auto& point : points) {
                float distance = pointToShift.euclideanDistance(point);
                if (distance <= radius) {
                    neighbors.emplace_back(distance, point);
                }
            }

            // 只保留最近的 k 个点
            std::sort(neighbors.begin(), neighbors.end(), [](const std::pair<float, Point>& a, const std::pair<float, Point>& b) {
                return a.first < b.first;
                });
            if (neighbors.size() > K) {
                neighbors.resize(K);
            }

            // 计算新位置
            for (const auto& neighbor : neighbors) {
                float gaussian = std::exp(-(neighbor.first * neighbor.first) / doubledSquaredBandwidth);
                newPosition += neighbor.second * gaussian;
                totalWeight += gaussian;
            }

            newPosition /= totalWeight;
            builder.shiftPoint(i, newPosition);
        }
        ++iterations;
    }

    if (iterations == MAX_ITERATIONS)
        std::cout << "WARNING: reached the maximum number of iterations" << std::endl;

    // 将新的位置应用回 cells
    for (long i = 0; i < points.size(); ++i) {
        cells[i].setPos(builder.getShiftedPoint(i));
    }

    
    buildClustersWithEpsilon(cells, bandwidth,clusters);
    
}

void buildClustersWithEpsilon(std::vector<Cell>& cells, float epsilon, vector<vector<Cell*>> &clusters) {
    cout << "clustering with epsilon\n";
    //std::vector<Cluster> clusters;
    //std::vector<bool> visited(cells.size(), false);
    //vector<vector<Cell *>> clusters;
    vector<int> connected(cells.size(), -1);
    //vector<int> boss(cells.size(), -1);
    
    
    for (int i = 0; i < cells.size(); ++i) {
        if (connected[i] == -1) {
            connected[i] = i;
        }
        int in_cluster_num = connected[i];

        for (int j = i+1; j < cells.size(); ++j) {
            if (cells[i].getPos().euclideanDistance(cells[j].getPos()) <= epsilon) {
                if (connected[j] == -1) {
                    connected[j] = in_cluster_num;
                }
                else {
                    int boss1 = findRoot(j, connected);
                    int boss2 = findRoot(i, connected);
                    if ( boss1 != boss2 ) {
                        connected[boss2] = boss1;
                    }
                }
            }
        }
    }
    vector<int> boss;
    for (int i = 0; i < connected.size(); ++i) {
        //vector<int>::iterator it = find(boss.begin(), boss.end(), findRoot(i, connected));
        bool create = true;
        for (int j = 0; j < boss.size();j++) {
            if (findRoot(i, connected) == boss[j]) {
                clusters[j].emplace_back(&cells[i]);
                create = false;
            }
        }
        if (create) {
            vector<Cell*> newCluster;
            newCluster.emplace_back(&cells[i]);
            clusters.emplace_back(newCluster);
            boss.emplace_back(i);
        }
    }
}
int findRoot(int target,vector<int> &connected) {
    if (connected[target] == target) {
        return target;
    }
    else {
        return findRoot(connected[target],connected);
    }
}
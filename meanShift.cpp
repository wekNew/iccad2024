#include <vector>
#include <iostream>
#include <cmath>
#include <omp.h>
#include "ClustersBuilder.h"
#include "meanShift.h"
#include <algorithm>
#define PI 3.1415926

struct Neighbor {
    float distance;
    Point point;
    float variance;
};

void meanShift(std::vector<shared_ptr<Cell>>& cells, float max_bandwidth, int M, int K, float epsilon, vector<vector<weak_ptr<Cell>>>& clusters) {
    cout << "start to meanShift\n";
    std::vector<Point> points;
    for (auto& cell : cells) {
        points.emplace_back(cell->getPos());
    }

    vector<float> final_bandwidths;
    if (M +1>= points.size()) {
        M = points.size() - 2;
    }
    for (int i = 0; i < points.size(); ++i) {

        Point current_point = points[i];
        vector<float> each_distance;
        for (int j = 0; j < points.size(); ++j) {
            float distance = current_point.euclideanDistance(points[j]);
            each_distance.emplace_back(distance);
        }
        sort(each_distance.begin(), each_distance.end());
        final_bandwidths.emplace_back(VariableBandwidth(max_bandwidth, each_distance[M + 1], cells[i]));
    }

    ClustersBuilder builder = ClustersBuilder(points, epsilon);
    long iterations = 0;
    unsigned long dimensions = points[0].dimensions();
    float radius = max_bandwidth * 3;
    //float doubledSquaredBandwidth = 2 * max_bandwidth * max_bandwidth;
    //Point history_pos[MAX_ITERATIONS];
    
    
    while (!builder.allPointsHaveStoppedShifting() && iterations < MAX_ITERATIONS) {
#pragma omp parallel for default(none) \
            shared(points, dimensions, builder, max_bandwidth, final_bandwidths, radius,iterations, cells, K) \
            schedule(dynamic)
        //cout << "\tcalculate shifted point\n";
        for (long i = 0; i < points.size(); ++i) {
            
            if (builder.hasStoppedShifting(i))
                continue;

            Point newPosition(dimensions);
            Point pointToShift = builder.getShiftedPoint(i);
            float totalWeight = 0.0;
            // 找到最近的 k 个点
            std::vector<Neighbor> neighbors;
            for (int j = 0; j < points.size(); ++j) {
                Point point = points[j];
                float distance = pointToShift.euclideanDistance(point);
                if (distance <= radius) {
                    Neighbor temp = { distance,point,final_bandwidths[j] };
                    neighbors.emplace_back(temp);
                }
            }
            // 只保留最近的 k 个点
            std::sort(neighbors.begin(), neighbors.end(), [](const Neighbor& a, const Neighbor& b) {
                return a.distance < b.distance;
                });
            if (neighbors.size() > K) {
                neighbors.resize(K);
            }
            /*
            int count = 0;
            for (auto c : neighbors) {
                cout << count << " distance : " << c.distance << ", variance : " << c.variance<<endl;
            }*/
            // 计算新位置
            for (const auto& neighbor : neighbors) {
                
                float gaussian = std::exp(-(neighbor.distance * neighbor.distance) / (2 * neighbor.variance * neighbor.variance));
                
                newPosition += neighbor.point * gaussian;
                totalWeight += gaussian;
            }
            newPosition /= totalWeight;
            if (iterations == 0) {
                newPosition = (newPosition + pointToShift * 9) / 10;
            }
            
            builder.shiftPoint(i, newPosition);
            #pragma omp critical
        }
        ++iterations;
    }



    if (iterations == MAX_ITERATIONS) {
        std::cout << "WARNING: reached the maximum number of iterations" << std::endl;
        int notfinish = 0;
        for (int i = 0; i < cells.size(); ++i) {
            if (!builder.hasStoppedShifting(i)) {
                //cout << "cell : " << cells[i].get_inst_name() << "still wanna shift" << endl;
                notfinish++;
            }
        }
        cout << "Total point still wanne shift : " << notfinish << endl;
        /*for (int i = 1; i < MAX_ITERATIONS; i++) {
            cout << "C102688 at iteration " << i << " from (" << history_pos[i-1].access_Values().at(0) << "," << history_pos[i - 1].access_Values().at(1) << ") to ("
                << history_pos[i].access_Values().at(0) << "," << history_pos[i].access_Values().at(1) << ")\tTotal moving : " << history_pos[i].euclideanDistance(history_pos[i - 1]) << endl;
        }*/
    }
    else {
        cout << "run to iteration : " << iterations << endl;
    }
        

    // 将新的位置应用回 cells
    for (long i = 0; i < points.size(); ++i) {
        cells[i]->setPos(builder.getShiftedPoint(i));
    }

    
    buildClustersWithEpsilon(cells, epsilon,clusters);
    
}
float VariableBandwidth(float max_distance,float Mth_distance,shared_ptr<Cell> cell) {
    cout << "\tVariableBandwidth=" << std::min(max_distance, (float)(atan(cell->get_min_slack() - 5) / PI + 0.5) * Mth_distance) << "( " <<Mth_distance<<","<<cell->get_min_slack()<<"," << (atan(cell->get_min_slack() - 5) / PI + 0.5)  << " )" << endl;
    return std::min(max_distance, (float)(atan(cell->get_min_slack()-5)/PI + 0.5) * Mth_distance);
}
void buildClustersWithEpsilon(std::vector<shared_ptr<Cell>>& cells, float epsilon, vector < vector<weak_ptr<Cell>>>& clusters) {
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
            if (cells[i]->getPos().euclideanDistance(cells[j]->getPos()) <= epsilon) {
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
                clusters[j].emplace_back(cells[i]);
                //clusters[j].add_cell(&cells[i]);
                create = false;
            }
        }
        if (create) {
            vector<weak_ptr<Cell>> newCluster;
            newCluster.emplace_back(cells[i]);
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
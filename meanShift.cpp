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

std::vector<Cluster> meanShift(std::vector<Cell>& cells, float bandwidth,int K) {
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
#pragma omp parallel for default(none) \
            shared(points, dimensions, builder, bandwidth, radius, doubledSquaredBandwidth, cells, k) \
            schedule(dynamic)
        for (long i = 0; i < points.size(); ++i) {
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

    // 使用更新后的 cells 来构建 clusters
    std::vector<Cluster> clusters;
    for (long i = 0; i < cells.size(); ++i) {
        Point shiftedPoint = cells[i].getPos();
        auto it = clusters.begin();
        auto itEnd = clusters.end();
        while (it != itEnd) {
            if (it->getCentroid().euclideanDistance(shiftedPoint) <= bandwidth) {
                // 将点添加到已经创建的簇中
                it->addPoint(cells[i].getPos());
                break;
            }
            ++it;
        }
        if (it == itEnd) {
            // 创建一个新的簇
            Cluster cluster(shiftedPoint);
            cluster.addPoint(cells[i].getPos());
            clusters.emplace_back(cluster);
        }
    }


    return clusters;
}

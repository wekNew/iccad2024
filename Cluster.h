#ifndef MEANSHIFT_CLUSTER_H
#define MEANSHIFT_CLUSTER_H

#include <vector>
#include <memory>
#include "Point.h"
#include "cell.h"
using namespace std;

class Cluster {
public:
    Cluster() = default;
    Cluster(const std::vector<shared_ptr<Cell>>& cells) : cells(cells) {}

    void FindCentroid() {
        Point temp = { 0.0, 0.0 };
        for (const auto& v : cells) {
            temp += v->getPos();
        }
        cluster_pos = temp / static_cast<float>(cells.size());
    }

    std::vector<shared_ptr<Cell>>& getCells() {
        return cells;
    }

    void add_cell(shared_ptr<Cell> cell) {
        cells.emplace_back(cell);
    }

    Point& getPos() {
        return cluster_pos;
    }

private:
    std::vector<shared_ptr<Cell>> cells;
    Point cluster_pos;
};

#endif
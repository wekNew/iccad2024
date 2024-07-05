#ifndef MEANSHIFT_CLUSTER_H
#define MEANSHIFT_CLUSTER_H

#include <vector>

#include "Point.h"
#include "cell.h"
using namespace std;
class Cluster {
public:
    // 纐粄篶硑ㄧ计
    Cluster() = default;
    /*
    // ī篶硑ㄧ计
    Cluster(const Cluster& other) : cells(other.cells), cluster_pos(other.cluster_pos) {}

    // 结巨才
    Cluster& operator=(const Cluster& other) {
        if (this != &other) {
            cells = other.cells;
            cluster_pos = other.cluster_pos;
        }
        return *this;
    }

    // 簿笆篶硑ㄧ计
    Cluster(Cluster&& other) noexcept : cells(std::move(other.cells)), cluster_pos(std::move(other.cluster_pos)) {}

    // 簿笆结巨才
    Cluster& operator=(Cluster&& other) noexcept {
        if (this != &other) {
            cells = std::move(other.cells);
            cluster_pos = std::move(other.cluster_pos);
        }
        return *this;
    }
    */
	void FindCentroid() {
		Point temp = { 0.0, 0.0 };
		for (const auto& v : cells) {
			temp += v->getPos();
		}
		cluster_pos = temp / static_cast<float>(cells.size());
	}
	std::vector<Cell*>& getCells(){
		return cells;
	}
	void add_cell(Cell* cell) {
		cells.emplace_back(cell);
	}
	Point& getPos(){
		return cluster_pos;
	}
private:
	std::vector<Cell*> cells;
	Point cluster_pos;
};
#endif MEANSHIFT_CLUSTER_HPP
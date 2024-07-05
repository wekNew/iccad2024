#ifndef MEANSHIFT_CLUSTER_H
#define MEANSHIFT_CLUSTER_H

#include <vector>

#include "Point.h"
#include "cell.h"
using namespace std;
class Cluster {
public:
    // �q�{�c�y���
    Cluster() = default;
    /*
    // �����c�y���
    Cluster(const Cluster& other) : cells(other.cells), cluster_pos(other.cluster_pos) {}

    // ��Ⱦާ@��
    Cluster& operator=(const Cluster& other) {
        if (this != &other) {
            cells = other.cells;
            cluster_pos = other.cluster_pos;
        }
        return *this;
    }

    // ���ʺc�y���
    Cluster(Cluster&& other) noexcept : cells(std::move(other.cells)), cluster_pos(std::move(other.cluster_pos)) {}

    // ���ʽ�Ⱦާ@��
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
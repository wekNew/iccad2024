#ifndef MEANSHIFT_CLUSTER_H
#define MEANSHIFT_CLUSTER_H

#include <vector>

#include "Point.h"
#include "cell.h"
using namespace std;
class Cluster {
public:
    // Àq»{ºc³y¨ç¼Æ
    Cluster() = default;
	Cluster(const std::vector<Cell*>& cells) : cells(cells) {}

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
#endif 
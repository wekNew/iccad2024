#pragma once
#ifndef MEANSHIFT_MEANSHIFT_H
#define MEANSHIFT_MEANSHIFT_H

#include <vector>

#include "Point.h"
#include "Cluster.h"
#include "cell.h"

#define MAX_ITERATIONS 100


std::vector<Cluster> meanShift(std::vector<Cell>& cells, float bandwidth,int K);


#endif //MEANSHIFT_MEANSHIFT_HPP
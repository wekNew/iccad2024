#pragma once
#ifndef	_PERTURBATION_H_
#define _PERTURBATION_H_

#include <vector>
#include <iostream>
#include "cell.h"
#include <algorithm>

struct CostFF {
	vector<Cell> cells;
	float cost;
};

void perturbation(vector<Cell>& FF, vector<Cell> standard_FF, int max_population, float mutation_ratio, int max_iteration);

void ChangeSameBitCell(Cell& target_cell, vector<Cell> standard_FF, vector<int> standard_bit_index);

#endif _PERTURBATION_H_
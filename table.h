#pragma once
#ifndef	_TABLE_H_
#define _TABLE_H_

#include "cell.h"
#include "cell.h"
#include <vector>

#define MAX_BIT 20
struct combination {
	//	string name;
	//	int power = INT_MAX;
	//	int area = INT_MAX;
	int combi_1 = -1;
	int combi_2 = -1;
	//int mix_num = 0;
	int cost = INT_MAX;
};

void buildTable(vector<Cell>& standard_FF, vector<combination>& combi_table, vector<Cell>& MBFF, vector<Cell*>& best_st_table, int beta, int gamma);

#endif _TABLE_H_
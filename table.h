#pragma once
#ifndef	_TABLE_H_
#define _TABLE_H_


#include "cell.h"
#include <vector>
#include <memory>

#define MAX_BIT 20
#define INT_MAX 2147483647

struct combination {
	//	string name;
	//	int power = INT_MAX;
	//	int area = INT_MAX;
	int combi_1 = -1;
	int combi_2 = -1;
	//int mix_num = 0;
	int cost = INT_MAX;
};

void buildCombiTable(vector<combination>& combi_table, vector<Cell*>& best_st_table, int beta, int gamma, int max_cluster_num);
void buildBestStTable(vector<Cell>& standard_FF, vector<Cell*>& best_st_table, int beta, int gamma);
#endif 
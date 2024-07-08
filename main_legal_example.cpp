#include "legalize.h"

using namespace std;


int main() {
	//讀取partition//

	/////////////////

	//MBFF　cell設定//
	//1bit slack:1.0 height:10.0 width:5.0
	//2bit slack:2.0 height:10.0 width:8.0
	//4bit slack:3.0 height:20.0 width:12.0	
	vector<cell>cells;
	cell bit_MBFF_1 = { 1, 1.0, 5, 10 };
	cell bit_MBFF_2 = { 2, 2.0, 8, 10 };
	cell bit_MBFF_4 = { 4, 3.0, 12, 20 };

	cells.push_back(bit_MBFF_1);
	cells.push_back(bit_MBFF_2);
	cells.push_back(bit_MBFF_4);
	//////////////////

	//cluster/////////
	//C0:c11, C1:c4,c20,c24,c26, C2:c2,c13,c17,c22, C3:c5 C4:c16,c21,
	//C5:c7,c14,c18,c28, C6:c1,c10, C7:c0,c3,c23,c27, C8:c6,c9,c15,c29, C9:c8,c12,c19,c25
	vector<cluster> clusters = { //bit,slack,x_ordinate,y_ordinate,cell_kind,cluster_name
		{1, 1.0, 0.0, 10.0, {bit_MBFF_1}, 0},
		{4, 3.0, 8.0, 20.0, {bit_MBFF_4}, 1},
		{4, 3.0, 0.0, 0.0, {bit_MBFF_4}, 2},
		{1, 1.0, 10.0, 90.0, {bit_MBFF_1}, 3},
		{2, 2.0, 6.0, 20.0, {bit_MBFF_2}, 4},
		{4, 3.0, 0.0, 0.0, {bit_MBFF_4}, 5},
		{2, 2.0, 14.0, 20.0, {bit_MBFF_2}, 6},
		{4, 3.0, 0.0, 50.0, {bit_MBFF_4}, 7},
		{4, 3.0, 35.0, 40.0, {bit_MBFF_4}, 8},
		{4, 3.0, 30.0, 80.0, {bit_MBFF_4}, 9}
	};
	//////////////////
	legalize(clusters);//只需要把partion轉換成cluster的形式即可legalize~~
	return 0;
 }
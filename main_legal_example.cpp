#include "legalize.h"
#include"Cell.h"

using namespace std;


int main() {

	vector<Cell> MBFF;
	//bit,width,height,Point pos,penalty(right,left,up,down),cluster_num
	MBFF.push_back({ 1, 5, 10, {0, 0}, 0, 0, 0, 0, 0 });
	MBFF.push_back({ 4, 12, 20, {5, 8}, 0, 0, 0, 0, 1 });
	MBFF.push_back({ 4, 12, 20, {6, 10}, 0, 0, 0, 0, 2 });
	MBFF.push_back({ 1, 5, 10, {7, 20}, 0, 0, 0, 0, 3 });
	MBFF.push_back({ 2, 8, 10,{ 9, 0}, 0, 0, 0, 0, 4 });
	MBFF.push_back({ 4, 12, 20, {10, 10}, 0, 0, 0, 0, 5 });
	MBFF.push_back({ 2, 8, 10, {50, 50}, 0, 0, 0, 0, 6});
	MBFF.push_back({ 4, 12, 20, {0, 10}, 0, 0, 0, 0, 7 });
	MBFF.push_back({ 4, 12, 20,{ 0, 20 }, 0, 0, 0, 0, 8});
	MBFF.push_back({ 4, 12, 20, {10, 0}, 0, 0, 0, 0, 9 });
	legalize(MBFF);//只需要把partion轉換成cluster的形式即可legalize~~
	return 0;
 }

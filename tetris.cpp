#include<cmath>
#include <unordered_map>
#include <unordered_set>
#include "tetris.h"
#include"cell.h"
#include"Point.h"
#include"pin.h"

using namespace std;

int BinMaxUtil = 75;
int binwidth = 0;
int binheight = 0;
int Die_max_x = 0;
int Die_max_y = 0;
int Die_min_x = 0;
int Die_min_y = 0;
int max_penalty = 10;

shared_ptr<Window> FindWindowForMBFF(shared_ptr<Cell> MBFF, vector<vector<shared_ptr<Window>>> windows, int window_start_x, int window_start_y, int unit_x, int unit_y) {
	vector<int> min_x_index, min_y_index;
	vector<int> max_x_index, max_y_index;

	for (auto v : MBFF->get_children()) {
		min_x_index.emplace_back(IndexConvert(v->getPos().get_xpos(), window_start_x, unit_x));
		min_y_index.emplace_back(IndexConvert(v->getPos().get_ypos(), window_start_y, unit_y));
		max_x_index.emplace_back(IndexConvert(v->getPos().get_xpos() + v->get_ff_width(), window_start_x, unit_x));
		max_y_index.emplace_back(IndexConvert(v->getPos().get_ypos() + v->get_ff_height(), window_start_y, unit_y));
	}
	/////////////////////////////////////////////////////////////////////////////
	int x_start_index = *std::min_element(min_x_index.begin(), min_x_index.end());
	int y_start_index = *std::min_element(min_y_index.begin(), min_y_index.end());
	int x_end_index = *std::min_element(max_x_index.begin(), max_x_index.end());
	int y_end_index = *std::min_element(max_y_index.begin(), max_y_index.end());
	return CombiWindow(windows, x_start_index, y_start_index, x_end_index, y_end_index);
}
shared_ptr<Window> CombiWindow(vector<vector<shared_ptr<Window>>> windows, int x_start_index, int y_start_index, int x_end_index, int y_end_index) {
	shared_ptr<Window> new_window=make_shared<Window>();
	int new_window_xpos = windows.at(x_start_index).at(y_start_index)->get_xpos();
	int new_window_ypos = windows.at(x_start_index).at(y_start_index)->get_ypos();
	int new_window_width = windows.at(x_end_index).at(y_start_index)->get_xpos() + windows.at(x_end_index).at(y_start_index)->get_width() - new_window_xpos;
	int new_window_height = windows.at(x_start_index).at(y_end_index)->get_xpos() + windows.at(x_start_index).at(y_end_index)->get_height() - new_window_ypos;
	new_window->set_window_size(new_window_xpos, new_window_ypos, new_window_width, new_window_height);

	for (int X = x_start_index; X <= x_end_index; ++X) {
		for (int Y = y_start_index; Y <= y_end_index; ++Y) {
			for (auto& v : windows.at(X).at(Y)->access_FF()) {
				new_window->access_FF().emplace_back(v);
			}
			for (auto& v : windows.at(X).at(Y)->access_Gate()) {
				new_window->access_Gate().emplace_back(v);
			}
			for (auto& v : windows.at(X).at(Y)->access_EdgeCell()) {
				if ((v->getPos().get_xpos() < new_window_xpos) || (v->getPos().get_ypos() < new_window_ypos) || (v->getPos().get_xpos() + v->get_ff_width() > new_window_xpos + new_window_width) || (v->getPos().get_ypos() + v->get_ff_height() > new_window_ypos + new_window_height)) {
					new_window->access_EdgeCell().emplace_back(v);
				}
				else {
					if (v->get_bit() == 0) {
						new_window->access_Gate().emplace_back(v);
					}
					else {
						new_window->access_FF().emplace_back(v);
					}
				}
			}
		}
	}
	return new_window;
}
void to_the_site(shared_ptr<Cell>& cell) {//到site上
	int x = cell->getPos().access_Values().at(0);
	int y = cell->getPos().access_Values().at(1);
	cout << "x:" << x << ",\ty:" << y;
	int new_x = x / binwidth;
	int new_y = y / binheight;
	x = binwidth * new_x;
	y = binheight * new_y;

	cout << ",\tafter shift,\tx:" << x << ",\ty:" << y;

	Point new_position({ static_cast<float>(x), static_cast<float>(y) });
	cell->setPos(new_position);
}
float poriority_cost_function(shared_ptr<Cell>& FF) {
	float cost;
	float slack = 0.0;
	float area;
	int ff_height=FF->get_ff_height()/binheight;
	for (int i = 0; i < FF->get_children().size(); i++) {
		shared_ptr<Cell> child = FF->get_children()[i];
		for (auto& pin : child->get_pin()) {
			slack += pin->get_timing_slack();
		}
	}
	slack = slack / FF->get_children().size();
	area = FF->get_ff_height() * FF->get_ff_width();
	cost = ff_height * 1000 + atan(area/slack);//arctan的cost function
	return cost;
}
bool poriority_bigger(shared_ptr<Cell>& FF1, shared_ptr<Cell>& FF2) {//比較cost function
	if (poriority_cost_function(FF1) <= poriority_cost_function(FF2)) {
		return true;
	}
	
	else return false;
}

void place_cell_into_interval(Window& WD, shared_ptr<Cell>& cell,int row) {
	if (cell->get_single_row_height() == true) {
		WD.set_row_interval(row,cell->get_xpos(),cell->get_xpos()+cell->get_ff_width());
	}
	else {
		int ff_row_height = cell->get_ff_height() / binheight;
		for (int i = row; i < ff_row_height; i++) {
			WD.set_row_interval(i, cell->get_xpos(), cell->get_xpos() + cell->get_ff_width());
		}
	}
}
void place_FF_into_interval(Window&WD,shared_ptr<Cell>& FF,int row,int FF_height,int time) {
	vector < Interval >check_interval = WD.get_valid_interval(row,FF_height);
	int x = FF->getPos().access_Values().at(0);
	int y = FF->getPos().access_Values().at(1);
	bool success = false;
	if (check_interval.size() != 0) {
		sort(check_interval.begin(), check_interval.end(), [x, FF](const Interval& a, const Interval& b) {//先照FF的距離去排列
			int dist_a, dist_b;
			if ((a.start - x) < a.end - (x + FF->get_ff_width())) {
				dist_a = a.start - x;
			}
			else {
				dist_a = a.end - (x + FF->get_ff_width());
			}
			if ((b.start - x) < b.end - (x + FF->get_ff_width())) {
				dist_b = b.start - x;
			}
			else {
				dist_b = b.end - (x + FF->get_ff_width());
			}
			return dist_a < dist_b;
			});
		for (int i = 0; i < check_interval.size(); i++) {
			if (check_interval[i].end - check_interval[i].start >= FF->get_ff_width()) {//有區間放得下去
				if (i == 0) {//一開始就可以放
					Point new_position_1({ static_cast<float>(x), static_cast<float>(y) });
					FF->setPos(new_position_1);
					place_cell_into_interval(WD, FF, row);
					success = true;
				}
				else {//移到那個區間
					if ((x - check_interval[i].start) < (check_interval[i].end - (x + FF->get_ff_width()))) {
						x = check_interval[i].start;
					}
					else {
						x = check_interval[i].end;
					}
					Point new_position_2({ static_cast<float>(x), static_cast<float>(y) });
					FF->setPos(new_position_2);
					place_cell_into_interval(WD, FF, row);
					success = true;
				}
			}
		}
	}
	else {
		success = false;
	}
	int window_y = WD.get_ypos();
	if (success == false) {//代表row放不下去
		time++;
		int up_down = time % 4;
		int move = time / 4;
		bool place_down;
		bool place_up;
		if (up_down == 1 || up_down == 2) {
			if (place_up != false) {
				y = FF->get_ypos() + binheight * (move * 2 + up_down);
				if (y <= WD.get_ypos() + WD.get_height() || y + FF->get_ff_height() <= WD.get_ypos() + WD.get_height()) {
					Point new_position_3({ static_cast<float>(x), static_cast<float>(y) });
					FF->setPos(new_position_3);
					row = (y - window_y) / binheight;
					place_FF_into_interval(WD, FF, row, FF_height, time);
					success = true;
				}
				else {
					place_up = false;
					success = false;
				}
			}			
		}
		else if (up_down == 3) {
			if (place_down != false) {
				y = FF->get_ypos() - binheight * (move * 2 + up_down - 2);
				if (y >= WD.get_ypos()) {
					Point new_position_4({ static_cast<float>(x), static_cast<float>(y) });
					FF->setPos(new_position_4);
					row = (y - window_y) / binheight;
					place_FF_into_interval(WD, FF, row, FF_height, time);
					success = true;
				}
				else {
					place_down == false;
					success = false;
				}
			}
			
		}
		else if (up_down == 0) {
			if (place_down != false) {
				y = FF->get_ypos() - binheight * (move * 2);
				if (y >= WD.get_ypos()) {
					Point new_position_4({ static_cast<float>(x), static_cast<float>(y) });
					FF->setPos(new_position_4);
					row = (y - window_y) / binheight;
					place_FF_into_interval(WD, FF, row, FF_height, time);
					success = true;
				}
				else {
					place_down = false;
					success = false;
				}
			}
		}
		if (success == false && place_up == false && place_down == false) {//完全放不下
			WD.set_illegal_FF(FF);
		}
		
	}
	
}
void sort_input(vector<shared_ptr<Cell>>& FF) {//依照poriortity去排FF
	sort(FF.begin(), FF.end(), poriority_bigger);//先依照poriority去排
	for (int i = 0; i < FF.size(); i++) {
		if (FF[i]->get_ff_height() / binwidth > 1) {//再判斷是不是single row height
			FF[i]->set_single_row_height(false);
		}
		else {
			FF[i]->set_single_row_height(true);
		}
	}
}

void tetris(Window& WD,vector<Cell>& MBFF, int bin_width, int bin_height, int die_x_min, int die_y_min, int die_x_max, int die_y_max) {
	int num_of_multi_row_height = 0;
	binwidth = bin_width;
	binheight = bin_height;
	Die_max_x = die_x_max;
	Die_max_y = die_y_max;
	Die_min_x = die_x_min;
	Die_min_y = die_y_min;
	cout << "unit: " << bin_width << ", " << bin_height << "\n";
	for (auto& cell : WD.access_Gate()) {
		to_the_site(cell);
		cout << "\n";
	}
	for (auto& cell : WD.access_EdgeCell()) {
		to_the_site(cell);
		cout << "\n";
	}
	for (auto& cell : WD.access_FF()) {
		to_the_site(cell);
		cout << "\n";
	}
	//先放gate
	int window_y = WD.get_ypos();
	for (int i = 0; i < WD.access_Gate().size(); i++) {
		shared_ptr<Cell> gate = WD.access_Gate()[i];
		int gate_x_start = gate->get_xpos();
		int gate_x_end = gate->get_xpos() + gate->get_ff_width();
		int row = (gate->get_ypos() - window_y) / binheight;
		place_cell_into_interval(WD, gate, row);
	}
	//再放edge cell,這邊是不會overlap的放法
	for (int i = 0; i < WD.access_EdgeCell().size(); i++) {
		shared_ptr<Cell> edge_cell = WD.access_Gate()[i];
		int gate_x_start = edge_cell->get_xpos();
		int gate_x_end = edge_cell->get_xpos() + edge_cell->get_ff_width();
		int row = (edge_cell->get_ypos() - window_y) / binheight;
		place_cell_into_interval(WD, edge_cell, row);
	}
	//最後放FF
	for (int i = 0; i < WD.access_FF().size(); i++) {//上面已經依照poriority去排好了
		int time = 0;
		shared_ptr<Cell> FF = WD.access_FF()[i];
		int FF_x_start = FF->get_xpos();
		int FF_x_end = FF->get_xpos() + FF->get_ff_width();
		int row = (FF->get_ypos() - window_y) / binheight;
		int gate_height = FF->get_ff_height() / binheight;
		place_FF_into_interval(WD, FF, row, gate_height,time);
	}
}

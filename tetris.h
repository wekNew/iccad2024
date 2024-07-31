//#pragma once
#ifndef TETRIS_H
#define TETRIS_H
#include <vector>
#include <algorithm>
#include "cell.h"

struct Interval {
	float start;
	float end;
};
struct IntervalPoint {
	float x_pos;
	int identity;//		1代表start	-1代表end;
};
struct OnsiteLocation {
	int startX;
	int startY;
	int siteWidth;
	int siteHeight;
	int totalNumOfSite;
};

class Window {
private:
	int x_pos, y_pos, height, width;
	vector<shared_ptr<Cell>> FF;
	vector<shared_ptr<Cell>> Gate;
	vector<shared_ptr<Cell>> EdgeCell;
	vector<vector<Interval>> row_interval;
	vector<OnsiteLocation*> row_onsite_pos;


	vector<Interval> intersection(int current_row, int row_height) {
		vector<Interval> final_interval;
		vector<IntervalPoint> temp;

		for (int i = current_row; i < current_row + row_height - 1; ++i) {
			if (i < row_interval.size()) {
				for (auto v : row_interval.at(i)) {
					IntervalPoint s = { v.start ,1 };
					temp.emplace_back(s);
					IntervalPoint e = { v.end,-1 };
					temp.emplace_back(e);

				}

			}
			else {
				cout << "Error : multi-row out of bounary\n";
			}
		}
		std::sort(temp.begin(), temp.end(), [](const IntervalPoint& a, const IntervalPoint& b) {
			return a.x_pos < b.x_pos;
			});
		int count = 0;
		bool record = false;
		Interval current_interval;


		for (auto v : temp) {
			count += v.identity;
			if (count == row_height) {
				current_interval.start = v.x_pos;
				record = true;
			}
			if (count == row_height - 1 && record) {
				current_interval.end = v.x_pos;
				final_interval.emplace_back(current_interval);
				record = false;
			}
		}
		return final_interval;
	}
public:
	int get_xpos() {
		return x_pos;
	}
	int get_ypos() {
		return y_pos;
	}
	int get_width() {
		return width;
	}
	int get_height() {
		return height;
	}
	vector<shared_ptr<Cell>>& access_FF() {
		return FF;
	}
	vector<shared_ptr<Cell>>& access_Gate() {
		return Gate;
	}
	vector<shared_ptr<Cell>>& access_EdgeCell() {
		return EdgeCell;
	}
	vector<Interval> get_valid_interval(int current_row, int row_height) {
		if (row_height == 1) {
			return row_interval.at(current_row);
		}
		return intersection(current_row, row_height);
	}

	////////////////////////////////////////////////////////////////////////
	void set_window_size(int x_pos, int y_pos, int width, int height) {
		this->x_pos = x_pos;
		this->y_pos = y_pos;
		this->width = width;
		this->height = height;
	}

};
inline int IndexConvert(float pos, int window_start_pos, int unit) {
	return ((pos - window_start_pos) / unit);
}
shared_ptr<Window> FindWindowForMBFF(shared_ptr<Cell> MBFF, vector<vector<shared_ptr<Window>>> windows, int window_start_x, int window_start_y, int unit_x, int unit_y);
shared_ptr<Window> CombiWindow(vector<vector<shared_ptr<Window>>> windows, int x_start_index, int y_start_index, int x_end_index, int y_end_index);



#endif
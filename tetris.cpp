#include "tetris.h"

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
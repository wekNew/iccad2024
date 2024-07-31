#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include <fstream>
#include <sstream>
using namespace std;
#include"pin.h"
#include"cell.h"
#include"netlist.h"
#include "meanShift.h"
#include "Cluster.h"
#include "table.h"
#include "partition.h"
#include "legalize.h"
#include"tetris.h"

#include <chrono>

#define START_TIMER(func) \
    auto func##_start = std::chrono::high_resolution_clock::now();

#define STOP_TIMER(func, description, logfile) \
    auto func##_end = std::chrono::high_resolution_clock::now(); \
    auto func##_duration = std::chrono::duration_cast<std::chrono::duration<double>>(func##_end - func##_start); \
    logfile << description << " execution time: " << func##_duration.count() << " seconds" << std::endl;
struct Die {
	int x_min, y_min, x_max, y_max;
	int input_pin_num, optput_pin_num;
	vector<shared_ptr<Pin>> input_pin, output_pin;
};

//string input_filename="testcase1_0614.txt";
string input_filename = "test.txt";
void input_file();
void initialize();
void show();
void show_cluster(vector < shared_ptr<Cluster>> clusters);
void show_MBFF();
void show_windows();
void drawPlot();

float costFunc(vector<Cell>& mbff);


int alpha, beta, Gamma, delta;
int bin_width, bin_height, bin_max_util;
int row_start_x, row_start_y, row_width, row_height;
float displacement_delay;

Die die;
//vector<Pin*> total_pin;
vector<Cell> standard_FF;
vector<shared_ptr<Cell>> FF;
vector<Cell> standard_Gate;
vector<shared_ptr<Cell>> Gate;

vector<vector<shared_ptr<Cell>>>same_clk_FF;

std::vector<std::shared_ptr<Cluster>> clusters;
vector <vector<weak_ptr<Cell>>> CLUSTERS;

vector<vector<shared_ptr<Window>>> windows;
vector<OnsiteLocation> placement_site;

NetList netlist;

vector<combination> combi_table(1);  //combinational table:use index to represent bit from 1 to MAXBIT
vector<shared_ptr<Cell>> MBFF;
vector<Cell*> best_st_table; //record lowest cost standard FF

float max_bandwidth, epsilon;
int K, M, window_width, window_height;

int main() {

	max_bandwidth = 100.0;
	K = 10;
	M = 4;
	epsilon = 0.1;
	window_width = 10;
	window_height = 5;

	std::ofstream logfile("execution_times.log");
	if (!logfile.is_open()) {
		std::cerr << "Failed to open log file." << std::endl;
		return 1;
	}

	START_TIMER(input_file)
		input_file();
	STOP_TIMER(input_file, "input_file()", logfile)

		START_TIMER(buildBestStTable)
		buildBestStTable(standard_FF, best_st_table, beta, Gamma);
	STOP_TIMER(buildBestStTable, "buildBestStTable()", logfile)

		START_TIMER(InitialDebanking)
		InitialDebanking(FF, best_st_table);
	STOP_TIMER(InitialDebanking, "InitialDebanking()", logfile)

		START_TIMER(initialize)
		initialize();
	STOP_TIMER(initialize, "initialize()", logfile)
		
		show();
	show_windows();

	for (int i = 0; i < same_clk_FF.size(); ++i) {
		cout << "clk " << i << "\t:\n";
		for (auto v : same_clk_FF.at(i)) {
			cout << "\tname : " << v->get_inst_name() << "\tpos : (" << v->getPos().get_xpos() << "," << v->getPos().get_ypos() << endl;
			cout << "\t\tpin : ";
			for (auto u : v->get_pin()) {
				cout << " " << u->get_belong()->get_inst_name() << "/" << u->get_pin_name();
			}
			cout << endl;
		}
	}

	int max_cluster_size = 0;
	START_TIMER(meanShift)
		//cout << "start to meanshift\n";
		for (auto v : same_clk_FF) {
			meanShift(v, max_bandwidth, M, K, epsilon, CLUSTERS);
			for (auto& v : CLUSTERS) {
				auto sharedCluster = std::make_shared<Cluster>();
				cout << "\n";
				for (auto& weakCell : v) {

					if (auto cell = weakCell.lock()) {
						cout << "push " << cell->get_inst_name() << endl;
						sharedCluster->getCells().push_back(cell);
					}
				}

				sharedCluster->FindCentroid();
				clusters.push_back(sharedCluster);

				if (v.size() > max_cluster_size) {
					max_cluster_size = v.size();
				}
			}
			CLUSTERS.clear();
		}
	

	STOP_TIMER(meanShift, "meanShift()", logfile)

		show_cluster(clusters);


	START_TIMER(buildCombiTable)
		buildCombiTable(combi_table, best_st_table, beta, Gamma, max_cluster_size);
	STOP_TIMER(buildCombiTable, "buildCombiTable()", logfile)

		for (int i = 0; i < combi_table.size(); i++) {
			cout << "index: " << i << " combi_1: " << combi_table[i].combi_1 << " combi_2: " << combi_table[i].combi_2 << endl;
		}

	START_TIMER(clusterToMBFF)
		for (auto& cluster : clusters) {
			cout << "cluster" << endl;
			cout << cluster->getCells().size() << endl;
			cout << combi_table[cluster->getCells().size()].combi_1 << endl;
			cout << combi_table[cluster->getCells().size()].combi_2 << endl;
			clusterToMBFF(best_st_table, cluster->getCells(), cluster->getPos(), combi_table, MBFF, combi_table[cluster->getCells().size()].combi_1, combi_table[cluster->getCells().size()].combi_2, FF.size());
			cout << "MBFF size is : " << MBFF.size() << endl;
		}
	STOP_TIMER(clusterToMBFF, "clusterToMBFF()", logfile);

	show_MBFF();

	START_TIMER(legal)
		for (int i = 0; i < MBFF.size(); i++) {
			MBFF[i]->set_clusterNum(i);
		}
	//legalize(MBFF, bin_width, bin_height, die.x_min, die.y_min, die.x_max, die.y_max);
	STOP_TIMER(legal, "legalize()", logfile)

		drawPlot();

	logfile.close();
	return 0;
}
void input_file() {
	cout << "start to read file\n";
	ifstream file(input_filename);
	if (!file.is_open()) {
		cout << "無法打開檔案" << endl;
		return;
	}
	string line;
	int count = 0;
	int min_x_start = 10000000, min_y_start = 100000000;
	while (getline(file, line)) {
		istringstream linestream(line);

		vector<string> tokens;
		string token;

		cout << "\tcurrent read to line " << count << " : " << line << endl;
		while (linestream >> token) {
			tokens.push_back(token);
		}
		/////////////////////////////////////////////////////////////////
		if (tokens[0] == "Alpha") {
			alpha = stoi(tokens[1]);
		}
		else if (tokens[0] == "Beta") {
			beta = stoi(tokens[1]);
		}
		else if (tokens[0] == "Gamma") {
			Gamma = stoi(tokens[1]);
		}
		else if (tokens[0] == "Delta") {
			delta = stoi(tokens[1]);
		}
		else if (tokens[0] == "DieSize") {

			die.x_min = stoi(tokens[1]);
			die.y_min = stoi(tokens[2]);
			die.x_max = stoi(tokens[3]);
			die.y_max = stoi(tokens[4]);
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "NumInput") {
			int time = stoi(tokens[1]);
			die.input_pin.reserve(die.input_pin.capacity() + time);
			while (time--) {
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}
				//Pin temp_pin = { tokens[1], stoi(tokens[2]), stoi(tokens[3]) };
				auto temp_pin = make_shared<Pin>(tokens[1], stoi(tokens[2]), stoi(tokens[3]));
				die.input_pin.push_back(temp_pin);
				//total_pin.reserve(total_pin.capacity() + 1);
				//total_pin.push_back(&die.input_pin.at(die.input_pin.size()-1));
			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "NumOutput") {

			int time = stoi(tokens[1]);
			die.output_pin.reserve(die.output_pin.capacity() + time);
			while (time--) {
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}

				//Pin temp_pin = { tokens[1], stoi(tokens[2]), stoi(tokens[3]) };
				auto temp_pin = make_shared<Pin>(tokens[1], stoi(tokens[2]), stoi(tokens[3]));
				die.output_pin.push_back(temp_pin);
				//total_pin.reserve(total_pin.capacity() + 1);
				//total_pin.push_back(&die.output_pin.at(die.output_pin.size() - 1));

			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "FlipFlop") {
			// 创建一个 Cell 对象并填充数据
			Cell temp_cell(stoi(tokens[1]), tokens[2], stoi(tokens[3]), stoi(tokens[4]), stoi(tokens[5]));

			int time = stoi(tokens[5]);
			while (time--) {
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}

				// 创建一个 Pin 对象并填充数据
				auto temp_pin = std::make_shared<Pin>(tokens[1], std::stoi(tokens[2]), std::stoi(tokens[3]));

				// 使用 temp_pin 填充 temp_cell
				temp_cell.set_pin(temp_pin);
			}

			// 将 temp_cell 添加到 standard_FF 中
			standard_FF.emplace_back(temp_cell);
		}
		else if (tokens[0] == "Gate") {
			// 创建一个 Cell 对象并填充数据
			Cell temp_cell(0, tokens[1], stoi(tokens[2]), stoi(tokens[3]), stoi(tokens[4]));

			int time = stoi(tokens[4]);
			while (time--) {
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}

				// 创建一个 Pin 对象并填充数据
				auto temp_pin = std::make_shared<Pin>(tokens[1], std::stoi(tokens[2]), std::stoi(tokens[3]));

				// 使用 temp_pin 填充 temp_cell
				temp_cell.set_pin(temp_pin);
			}

			// 将 temp_cell 添加到 standard_Gate 中
			standard_Gate.emplace_back(temp_cell);
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "NumInstances") {
			int time = stoi(tokens[1]);
			//FF.reserve(FF.capacity() + time);
			while (time--) {
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}
				bool find = false;
				for (auto& v : standard_FF) {
					if (v.get_ff_name() == tokens[2]) {
						
						shared_ptr<Cell> temp_cell = make_shared<Cell>(0,"initial",0,0,0);
						temp_cell->inherit_data(v);
						temp_cell->set_inst(tokens[1], stof(tokens[3]), stof(tokens[4]));

						for (auto& u : temp_cell->get_pin()) {
							u->set_belong(temp_cell);
							cout << "u.get_belong().get_inst_name() : " << u->get_belong()->get_inst_name() << endl;
						}

						FF.emplace_back(temp_cell);
						find = true;
						break;
						
					}
				}
				if (!find) {
					for (auto& v : standard_Gate) {
						if (v.get_ff_name() == tokens[2]) {
							
							shared_ptr<Cell> temp_cell = make_shared<Cell>(0,"initial",0,0,0);
							temp_cell->inherit_data(v);
							temp_cell->set_inst(tokens[1], stof(tokens[3]), stof(tokens[4]));

							for (auto& u : temp_cell->get_pin()) {
								u->set_belong(temp_cell);
								cout << "u.get_belong().get_inst_name() : " << u->get_belong()->get_inst_name() << endl;
							}

							Gate.emplace_back(temp_cell);
							find = true;
							break;
							
						}
					}
				}

			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "NumNets") {
			netlist.set_net_count(stoi(tokens[1]));
			int time = stoi(tokens[1]);

			while (time--) {//deal with net_count
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}

				//Net temp_net = { tokens[1],stoi(tokens[2]) };
				auto temp_net = make_shared<Net>(tokens[1], stoi(tokens[2]));
				int pin_num = stoi(tokens[2]);
				while (pin_num--) {//deal with pin_count
					getline(file, line);
					istringstream linestream(line);
					tokens.clear();
					while (linestream >> token) {
						tokens.push_back(token);
					}

					//string after = tokens[1];
					size_t pos = tokens[1].find('/');

					if (pos != string::npos) {
						string before = tokens[1].substr(0, pos); // 提取分割字元之前的部分
						string after = tokens[1].substr(pos + 1);
						for (auto& v : FF) {
							if (v->get_inst_name() == before) {
								for (auto& u : v->get_pin()) {
									if (u->get_pin_name() == after) {
										cout << "Find Net with cell : " << v->get_inst_name() <<" / " << u->get_pin_name() <<" belong to " <<u->get_belong()->get_inst_name()<< endl;
										temp_net->add_pin(u);
										if (after == "clk") {
											cout << "\tBefore set_clk_net: Pin's clk_net is " << (u->get_clk_net() ? u->get_clk_net()->get_net_name() : "null") << endl;
											u->set_clk_net(temp_net);
											cout << "\tAfter set_clk_net: Pin's clk_net is " << (u->get_clk_net() ? u->get_clk_net()->get_net_name() : "null") << endl;
										}

										break;
									}
								}
							}
						}
					}
					else {
						for (auto& v : die.input_pin) {
							if (v->get_pin_name() == tokens[1]) {
								//cout << "Here\n";
								temp_net->add_pin(v);
								break;
							}
						}
						for (auto& v : die.output_pin) {
							if (v->get_pin_name() == tokens[1]) {
								//cout << "Here\n";
								temp_net->add_pin(v);
								break;
							}
						}
					}


				}
				netlist.set_net(temp_net);
			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "BinWidth") {
			bin_width = stoi(tokens[1]);
		}
		else if (tokens[0] == "BinHeight") {
			bin_height = stoi(tokens[1]);
		}
		else if (tokens[0] == "BinMaxUtil") {
			bin_max_util = stoi(tokens[1]);
		}
		else if (tokens[0] == "PlacementRows") {
			OnsiteLocation temp = { stoi(tokens[1]),stoi(tokens[2]),stoi(tokens[3]),stoi(tokens[4]),stoi(tokens[5]) };
			placement_site.emplace_back(temp);
			if (min_x_start > stoi(tokens[1])) {
				min_x_start = stoi(tokens[1]);
			}
			if (min_y_start > stoi(tokens[2])) {
				min_y_start = stoi(tokens[2]);
			}
			//row_start_x = stoi(tokens[1]);
			//row_start_y = stoi(tokens[2]);
			row_width = stoi(tokens[3]);
			row_height = stoi(tokens[4]);

		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "DisplacementDelay") {
			displacement_delay = stof(tokens[1]);
		}
		else if (tokens[0] == "QpinDelay") {
			for (auto& v : standard_FF) {
				if (v.get_ff_name() == tokens[1]) {

					v.set_q(stoi(tokens[2]));

					break;
				}
			}
			for (auto& v : FF) {
				if (v->get_ff_name() == tokens[1]) {
					v->set_q(stoi(tokens[2]));
				}
			}
		}
		else if (tokens[0] == "TimingSlack") {
			for (auto& v : FF) {
				if (v->get_inst_name() == tokens[1]) {
					for (auto& iter : v->get_pin()) {
						if (iter->get_pin_name() == tokens[2]) {
							iter->set_timing_slack(stof(tokens[3]));
							cout << "set_timing_slack=" << iter->get_timing_slack() << endl;
							break;
						}
					}
					break;
				}
			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "GatePower") {
			for (auto& v : standard_FF) {
				if (v.get_ff_name() == tokens[1]) {
					v.set_power(stof(tokens[2]));
					break;
				}
			}
			for (auto& v : FF) {
				if (v->get_ff_name() == tokens[1]) {
					v->set_power(stof(tokens[2]));
				}
			}
		}
		/////////////////////////////////////////////////////////////////
		count++;
		//file.ignore(numeric_limits<streamsize>::max(), '\n');
		fflush(stdin);
	}
	row_start_x = min_x_start;
	row_start_y = min_y_start;
	cout << "end of read file\n";
	file.close();
	return;
}
void initialize() {
	cout << "start to initialize\n";
	for (auto& v : netlist.get_contain_net()) {
		vector<shared_ptr<Cell>> temp;
		temp.clear();
		/*
		for (int i = 0; i < v.get_connect_pin().size(); ++i) {
			cout << "pin name = " << v.get_connect_pin().at(i)->get_pin_name() << endl;
		}
		*/
		for (auto& u : v->get_connect_pin()) {
			cout << "u pin name = " << u->get_pin_name() << endl;
			if (u->get_pin_name() == "clk") {
				shared_ptr<Cell> cellPtr = u->get_belong();
				if (cellPtr != nullptr) {
					std::cout << u->get_pin_name()<<" belong_emplace -> "<<u->get_belong()->get_inst_name()<<"\n";
					temp.emplace_back(cellPtr);
				}
			}

		}

		if (temp.size() != 0) {
			same_clk_FF.emplace_back(temp);
		}
	}
	std::cout << "start to bulid windows\n";
	for (int X = row_start_x; X < die.x_max; X += row_width * window_width) {
		vector<shared_ptr<Window>> row_window;
		int x_end = X + row_width * window_width;
		if (x_end >= die.x_max) {
			x_end = die.x_max;
		}
		for (int Y = row_start_y; Y < die.y_max; Y += row_height * window_height) {
			shared_ptr<Window> new_window=make_shared<Window>();
			int y_end = Y + row_height * window_height;
			if (y_end > die.y_max) {
				y_end = die.y_max;
			}
			new_window->set_window_size(X, Y, x_end - X, y_end - Y);
			row_window.emplace_back(new_window);

		}
		windows.emplace_back(row_window);
	}
	

	for (auto& v : FF) {

		int cell_x_min = v->getPos().access_Values().at(0);
		int cell_x_max = v->getPos().access_Values().at(0) + v->get_ff_width();
		int cell_y_min = v->getPos().access_Values().at(1);
		int cell_y_max = v->getPos().access_Values().at(1) + v->get_ff_height();
		int x_index = (cell_x_min - row_start_x) / (row_width * window_width);
		int y_index = (cell_x_min - row_start_y) / (row_height * window_height);
		cout << "x_index : " << x_index << "\ty_index : " << y_index << endl;
		shared_ptr<Window> current_window = windows.at(x_index).at(y_index);
		bool right_over = false, up_over = false;
		if (cell_x_max > current_window->get_xpos() + current_window->get_width()) {
			right_over = true;
		}
		if (cell_y_max > current_window->get_ypos() + current_window->get_height()) {
			up_over = true;
		}
		if (right_over) {
			windows.at(x_index + 1).at(y_index)->access_EdgeCell().emplace_back(v);
		}
		if (up_over) {
			windows.at(x_index).at(y_index + 1)->access_EdgeCell().emplace_back(v);
		}
		if (right_over && up_over) {

			windows.at(x_index + 1).at(y_index + 1)->access_EdgeCell().emplace_back(v);
		}
		if (!right_over && !up_over) {
			current_window->access_FF().emplace_back(v);
		}
		else {
			current_window->access_EdgeCell().emplace_back(v);
		}
	}
	for (auto& v : Gate) {

		int cell_x_min = v->getPos().access_Values().at(0);
		int cell_x_max = v->getPos().access_Values().at(0) + v->get_ff_width();
		int cell_y_min = v->getPos().access_Values().at(1);
		int cell_y_max = v->getPos().access_Values().at(1) + v->get_ff_height();
		int x_index = (cell_x_min - row_start_x) / (row_width * window_width);
		int y_index = (cell_x_min - row_start_y) / (row_height * window_height);
		cout << "x_index : " << x_index << "\ty_index : " << y_index << endl;
		shared_ptr<Window> current_window = windows.at(x_index).at(y_index);
		bool right_over = false, up_over = false;
		if (cell_x_max > current_window->get_xpos() + current_window->get_width()) {
			right_over = true;
		}
		if (cell_y_max > current_window->get_ypos() + current_window->get_height()) {
			up_over = true;
		}
		if (right_over) {
			windows.at(x_index + 1).at(y_index)->access_EdgeCell().emplace_back(v);
		}
		if (up_over) {
			windows.at(x_index).at(y_index + 1)->access_EdgeCell().emplace_back(v);
		}
		if (right_over && up_over) {

			windows.at(x_index + 1).at(y_index + 1)->access_EdgeCell().emplace_back(v);
		}
		if (!right_over && !up_over) {
			current_window->access_Gate().emplace_back(v);
		}
		else {
			current_window->access_EdgeCell().emplace_back(v);
		}
	}
	cout << "end of initialize\n";
}
void show() {
	show_stardard_FF(standard_FF);
	show_FF(FF);
	show_netlist(netlist);

}
void show_cluster(vector<shared_ptr<Cluster>> clusters) {
	ofstream outFile("show_clusters_after_EMS.txt");
	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}

	outFile << "size of clusters = " << clusters.size() << endl;
	int count = 0;

	for (auto& v : clusters) {
		auto clusterPtr = v;
		if (clusterPtr) { // 检查指针是否有效
			outFile << "cluster " << count << " :\n";
			for (auto& u : clusterPtr->getCells()) {
				outFile << "\t" << u->get_inst_name() << " : ";
				for (auto z : u->getPos()) {
					outFile << z << " ";
				}
				outFile << "\n";
			}
			outFile << "\n";
			count++;
		}
		else {
			outFile << "cluster " << count << " : (無法取得數據，指针已失效)\n";
			count++;
		}
	}
}

void show_MBFF() {
	cout << "start to show MBFF\n";
	ofstream outFile("show_MBFF.txt");
	if (!outFile.is_open()) {
		std::cerr << "can't open the file" << std::endl;
		return;
	}

	for (const auto& v : MBFF) {
		outFile << v->get_inst_name() << ":\n"
			<< "\tlib: \t" << v->get_ff_name() << endl
			<< "\tpos: \t" << v->getPos().access_Values().at(0) << " " << v->getPos().access_Values().at(1) << endl
			<< "\tsize:\t" << v->get_ff_width() << " " << v->get_ff_height() << endl
			<< "\tfrom:\t";
		for (const auto& u : v->get_children()) {
			outFile << u->get_inst_name() << " ";
		}
		outFile << endl;
	}


}
void show_windows() {
	cout << "start to show windows\n";
	ofstream outFile("show_window.txt");
	if (!outFile.is_open()) {
		std::cerr << "can't open the file" << std::endl;
		return;
	}
	int x_index = 0;
	for (auto v : windows) {
		int y_index = 0;
		for (auto u : v) {
			outFile << "Window at [ " << x_index << " , " << y_index << "] :\n";
			outFile << "\tpos : (" << u->get_xpos() << " , " << u->get_ypos() << " )\n";
			outFile << "\twidth : " << u->get_width() << "\theight : " << u->get_height() << endl;
			outFile << "\tFF :";
			for (auto cell : u->access_FF()) {
				outFile << " " << cell->get_inst_name();
			}
			outFile << endl;
			outFile << "\tGate :";
			for (auto cell : u->access_Gate()) {
				outFile << " " << cell->get_inst_name();
			}
			outFile << endl;
			outFile << "\tEdgeCell :";
			for (auto cell : u->access_EdgeCell()) {
				outFile << " " << cell->get_inst_name();
			}
			outFile << endl;
			y_index++;
		}
		x_index++;
	}
}
void drawPlot() {
	cout << "start to generate plot's dpx\n";
	ofstream outFile("my_project.dpx");
	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}
	for (auto v : MBFF) {
		outFile << "COLOR green" << endl;
		outFile << "SRF " << v->getPos().get_xpos() << " " << v->getPos().get_ypos() << " " << v->getPos().get_xpos() + v->get_ff_width() << " " << v->getPos().get_ypos() + v->get_ff_height() << endl;
	}
}

float costFunc(vector<Cell>& mbff)
{
	int die_width = die.x_max - die.x_min;
	int die_height = die.y_max - die.y_min;
	int bin_area = bin_width * bin_height;
	vector<vector<double>> bins_util(ceil((double)die_width / (double)bin_width), vector<double>(ceil((double)die_height / (double)bin_height), 0));

	float cost = 0;



	for (auto& reg : mbff) {
		cost += Gamma * reg.get_ff_height() * reg.get_ff_width();
		cost += beta * reg.get_power();

		int left_x = (reg.getPos().access_Values().at(0) - die.x_min);
		int right_x = (reg.getPos().access_Values().at(0) + reg.get_ff_width() - die.x_min);
		int down_y = (reg.getPos().access_Values().at(1) - die.y_min);
		int up_y = (reg.getPos().access_Values().at(1) + reg.get_ff_height() - die.y_min);

		int start_row = floor((float)left_x / (float)bin_width);
		int end_row = ceil((float)right_x / (float)bin_width) - 1;
		int start_col = floor((float)down_y / (float)bin_height);
		int end_col = ceil((float)up_y / (float)bin_height) - 1;


		for (int r = start_row; r <= end_row; ++r) {
			for (int c = start_col; c <= end_col; ++c) {
				int intersect_width = min((r + 1) * bin_width, right_x) - max(r * bin_width, left_x);
				int intersect_height = min((c + 1) * bin_height, up_y) - max(c * bin_height, down_y);
				int intersect_area = intersect_width * intersect_height;
				double usage_ratio = (double)(intersect_area) / (double)bin_area;
				bins_util.at(r).at(c) += usage_ratio;
			}
		}
	}

	for (int i = 0; i < bins_util.size(); i++) {
		for (int j = 0; j < bins_util.at(0).size(); j++) {
			cout << bins_util.at(i).at(j) << " ";
			if (bins_util.at(i).at(j) > (bin_max_util / 100)) cost += delta;
		}
		cout << endl;
	}

	return cost;
}
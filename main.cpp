#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include <fstream>
#include <sstream>
using namespace std;
#include"argument.h"
#include"pin.h"
#include"die.h"
#include"cell.h"
#include"netlist.h"

string input_filename="demo.txt";

void show_argument() {
	ofstream outFile("show_argument.txt");

	if (!outFile.is_open()) {
		std::cerr << "µLªk¥´¶}ÀÉ®×" << std::endl;
		return;
	}
	
	outFile << "Alpha  " << alpha << endl;
	outFile << "Beta  " << beta << endl;
	outFile << "Gamma  " << gamma << endl;
	outFile << "Delta  " << delta << endl;
	outFile << "Bin_width  " << bin_width << endl;
	outFile << "Bin_height  " << bin_height << endl;
	outFile << "Bin_max_util  " << bin_max_util << endl;
	outFile << "Row_start_x  " << row_start_x << endl;
	outFile << "Row_start_y  " << row_start_y << endl;
	outFile << "Row_width  " << row_width << endl;
	outFile << "Row_height  " << row_height << endl;
	outFile << "Displacement_delay  " << displacement_delay << endl;

	
	outFile.close();
	return;
}
void input_file();
void initialize();
void show();


int alpha, beta, gamma, delta;
int bin_width, bin_height, bin_max_util;
int row_start_x, row_start_y, row_width, row_height;
float displacement_delay;

Die die;
vector<Pin*> total_pin;
vector<Cell> standard_FF;
vector<Cell> FF;
NetList netlist;
int main() {
	input_file();
	initialize();
	show();
	return 0;
}
void input_file() {
	ifstream file(input_filename);
	if (!file.is_open()) {
		cout << "無法打開檔案" << endl;
		return;
	}
	string line;
	while (getline(file, line)) {
		istringstream linestream(line);

		vector<string> tokens;
		string token;
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
			gamma = stoi(tokens[1]);
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
				Pin temp_pin = { tokens[1], stoi(tokens[2]), stoi(tokens[3])};
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
				Pin temp_pin = { tokens[1], stoi(tokens[2]), stoi(tokens[3])};
				die.output_pin.push_back(temp_pin);
				//total_pin.reserve(total_pin.capacity() + 1);
				//total_pin.push_back(&die.output_pin.at(die.output_pin.size() - 1));
			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "FlipFlop") {
			standard_FF.reserve(standard_FF.capacity()+1);
			Cell temp_cell = { stoi(tokens[1]),tokens[2],stoi(tokens[3]) ,stoi(tokens[4]) ,stoi(tokens[5]) };
			
			int time = stoi(tokens[5]);
			while (time--) {
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}
				Pin temp_pin = { tokens[1], stoi(tokens[2]), stoi(tokens[3]) };
				temp_cell.set_pin(temp_pin);
				//total_pin.reserve(total_pin.capacity() + 1);
				//total_pin.push_back(&temp_cell.ff_pin.at(temp_cell.ff_pin.size()-1));
			}
			standard_FF.push_back(temp_cell);
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "NumInstances") {
			int time = stoi(tokens[1]);
			FF.reserve(FF.capacity() + time);
			while (time--) {
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}
				for (auto v : standard_FF) {
					if (v.ff_name == tokens[2]) {
						Cell temp_cell = v;
						temp_cell.set_inst(tokens[1], stoi(tokens[3]), stoi(tokens[4]));
						FF.push_back(temp_cell);
						break;
					}
				}
			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "NumNets") {
			netlist.net_count= stoi(tokens[1]);
			int time = stoi(tokens[1]);
			
			while (time--) {//deal with net_count
				getline(file, line);
				istringstream linestream(line);
				tokens.clear();
				while (linestream >> token) {
					tokens.push_back(token);
				}
				
				Net temp_net = { tokens[1],stoi(tokens[2]) };
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
							if (v.inst_name == before) {
								for (auto& u : v.ff_pin) {
									if (u.pin_name == after) {
										cout << "There\n";
										temp_net.set_pin(&u);
										break;
									}
								}
							}
						}
					}
					else {
						for (auto& v : die.input_pin) {
							if (v.pin_name == tokens[1]) {
								cout << "Here\n";
								temp_net.set_pin(&v);
								break;
							}
						}
						for (auto& v : die.output_pin) {
							if (v.pin_name == tokens[1]) {
								cout << "Here\n";
								temp_net.set_pin(&v);
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
			row_start_x = stoi(tokens[1]);
			row_start_y = stoi(tokens[2]);
			row_width = stoi(tokens[3]);
			row_height = stoi(tokens[4]);
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "DisplacementDelay") {
			displacement_delay = stof(tokens[1]);
		}
		else if (tokens[0] == "QpinDelay") {
			for (auto &v : standard_FF) {
				if (v.ff_name == tokens[1]) {
					
					v.q_pin_delay=stoi(tokens[2]);
					
					break;
				}
			}
			for (auto &v : FF) {
				if (v.ff_name == tokens[1]) {
					v.q_pin_delay = stoi(tokens[2]);
				}
			}
		}
		else if (tokens[0] == "TimingSlack") {
			for (auto &v : FF) {
				if (v.inst_name == tokens[1]) {
					for (auto &iter : v.ff_pin) {
						if (iter.pin_name == tokens[2]) {
							iter.timing_slack = stoi(tokens[3]);
							break;
						}
					}
					break;
				}
			}
		}
		/////////////////////////////////////////////////////////////////
		else if (tokens[0] == "GatePower") {
			for (auto &v : standard_FF) {
				if (v.ff_name == tokens[1]) {
					v.gate_power = stoi(tokens[2]);
					break;
				}
			}
			for (auto &v : FF) {
				if (v.ff_name == tokens[1]) {
					v.gate_power = stoi(tokens[2]);
				}
			}
		}
		/////////////////////////////////////////////////////////////////
	}

	file.close();
	return;
}
void initialize() {
	for (auto& v : die.input_pin) {
		total_pin.reserve(total_pin.size() + 1);
		total_pin.push_back(&v);
	}
	for (auto& v : die.output_pin) {
		total_pin.reserve(total_pin.size() + 1);
		total_pin.push_back(&v);
	}
	for (auto& v : FF) {
		for (auto& u : v.ff_pin) {
			u.belong = &v;
			total_pin.reserve(total_pin.size() + 1);
			total_pin.push_back(&u);
		}
	}

}
void show() {
	show_argument();
	show_stardard_FF(standard_FF);
	show_FF(FF);
	show_netlist(netlist);
}


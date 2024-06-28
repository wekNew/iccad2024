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
struct Die {
	int x_min, y_min, x_max, y_max;
	int input_pin_num, optput_pin_num;
	vector<Pin> input_pin, output_pin;
};

string input_filename="testcase1_0614.txt";

void input_file();
void initialize();
void show();
void show_cluster(vector<vector<Cell*>> clusters);

int alpha, beta, gamma, delta;
int bin_width, bin_height, bin_max_util;
int row_start_x, row_start_y, row_width, row_height;
float displacement_delay;

Die die;
vector<Pin*> total_pin;
vector<Cell> standard_FF;
vector<Cell> FF;
vector<vector<Cell*>> clusters;
NetList netlist;
int main() {
	input_file();
	initialize();
	show();

	float bandwidth = 100.0;
	int K = 40;
	
	meanShift(FF, bandwidth,K,clusters);

	// 打印結果
	cout << "\n";
	for (auto v : FF) {
		cout << v.get_inst_name() << "'s pos: ";
		for (auto u : v.getPos()) {
			cout << u<<" ";
		}
		cout << "\n";
	}
	
	/*for (auto v : clusters) {
		cout <<"cluster " << count<<" :\n";
		int p = 0;
		for (auto u : v) {
			cout << "\tpoint " << p << " : ";
			for (auto z : u) {
				cout  << z << " ";
			}
			cout << "\n";
			p++;
		}
		cout << "\n";
		count++;
	}*/
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
	while (getline(file, line)) {
		istringstream linestream(line);

		vector<string> tokens;
		string token;
		if (count % 2 == 0) {
			cout << "\tcurrent read to line " << count<<endl;
		}
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
		else if (tokens[0] == "Gate") {
			standard_FF.reserve(standard_FF.capacity() + 1);
			Cell temp_cell = { 0,tokens[1],stoi(tokens[2]) ,stoi(tokens[3]) ,stoi(tokens[4]) };

			int time = stoi(tokens[4]);
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
					if (v.get_ff_name() == tokens[2]) {
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
			netlist.set_net_count(stoi(tokens[1]));
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
							if (v.get_inst_name() == before) {
								for (auto& u : v.get_pin()) {
									if (u.get_pin_name() == after) {
										//cout << "There\n";
										temp_net.set_pin(&u);
										break;
									}
								}
							}
						}
					}
					else {
						for (auto& v : die.input_pin) {
							if (v.get_pin_name() == tokens[1]) {
								//cout << "Here\n";
								temp_net.set_pin(&v);
								break;
							}
						}
						for (auto& v : die.output_pin) {
							if (v.get_pin_name() == tokens[1]) {
								//cout << "Here\n";
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
				if (v.get_ff_name() == tokens[1]) {
					
					v.set_q(stoi(tokens[2]));
					
					break;
				}
			}
			for (auto &v : FF) {
				if (v.get_ff_name() == tokens[1]) {
					v.set_q(stoi(tokens[2]));
				}
			}
		}
		else if (tokens[0] == "TimingSlack") {
			for (auto &v : FF) {
				if (v.get_inst_name() == tokens[1]) {
					for (auto &iter : v.get_pin()) {
						if (iter.get_pin_name() == tokens[2]) {
							iter.set_timing_slack(stoi(tokens[3]));
							cout << "set_timing_slack=" << iter.get_timing_slack();
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
				if (v.get_ff_name() == tokens[1]) {
					v.set_power(stoi(tokens[2]));
					break;
				}
			}
			for (auto &v : FF) {
				if (v.get_ff_name() == tokens[1]) {
					v.set_power(stoi(tokens[2]));
				}
			}
		}
		/////////////////////////////////////////////////////////////////
		count++;
	}
	cout << "end of read file\n";
	file.close();
	return;
}
void initialize() {
	cout << "start to initialize\n";
	for (auto& v : die.input_pin) {
		total_pin.reserve(total_pin.size() + 1);
		total_pin.push_back(&v);
	}
	for (auto& v : die.output_pin) {
		total_pin.reserve(total_pin.size() + 1);
		total_pin.push_back(&v);
	}
	for (auto& v : FF) {
		for (auto& u : v.get_pin()) {
			u.set_belong(&v);
			total_pin.reserve(total_pin.size() + 1);
			total_pin.push_back(&u);
		}
	}

}
void show() {
	
	show_stardard_FF(standard_FF);
	show_FF(FF);
	show_netlist(netlist);
	show_cluster(clusters);
}
void show_cluster(vector<vector<Cell*>> clusters) {
	ofstream outFile("show_clusters_after_EMS.txt");
	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}
	outFile << "size of clusters = " << clusters.size() << endl;
	int count = 0;
	for (auto v : clusters) {
		outFile << "cluster " << count << " :\n";
		for (auto u : v) {
			outFile << "\t" << u->get_inst_name() << " : ";
			for (auto z : u->getPos()) {
				outFile << z << " ";
			}
			outFile << "\n";
		}
		outFile << "\n";
		count++;
	}
}


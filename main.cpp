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
	vector<Pin> input_pin, output_pin;
};

//string input_filename="testcase1_0614.txt";
string input_filename = "test.txt";
void input_file();
void initialize();
void show();
void show_cluster(vector < Cluster*> clusters);

int alpha, beta, gamma, delta;
int bin_width, bin_height, bin_max_util;
int row_start_x, row_start_y, row_width, row_height;
float displacement_delay;

Die die;
vector<Pin*> total_pin;
vector<Cell> standard_FF;
vector<Cell> FF;
vector<Cell> standard_Gate;
vector<Cell> Gate;

vector<Cluster*> clusters;
vector < vector < Cell* >> CLUSTERS;

NetList netlist;

vector<combination> combi_table(1);  //combinational table:use index to represent bit from 1 to MAXBIT
vector<Cell> MBFF;
vector<Cell*> best_st_table; //record lowest cost standard FF

int main() {

	float max_bandwidth = 6.0;
	int K = 10;
	int M = 4;
	float epsilon = 0.1;

	std::ofstream logfile("execution_times.log");
	if (!logfile.is_open()) {
		std::cerr << "Failed to open log file." << std::endl;
		return 1;
	}

	START_TIMER(input_file)
		input_file();
	STOP_TIMER(input_file,"input_file()", logfile)
	
	START_TIMER(initialize)
		initialize();
	STOP_TIMER(initialize, "initialize()", logfile)

		//show();


	START_TIMER(buildBestStTable)
		buildBestStTable(standard_FF, best_st_table, beta, gamma);
	STOP_TIMER(buildBestStTable,"buildBestStTable()", logfile)

	START_TIMER(InitialDebanking)
		InitialDebanking(FF,best_st_table);
	STOP_TIMER(InitialDebanking,"InitialDebanking()", logfile)

	
	
	START_TIMER(meanShift)
	meanShift(FF, max_bandwidth,M,K,epsilon,CLUSTERS);
	STOP_TIMER(meanShift, "meanShift()", logfile)

	int max_cluster_size = 0;
	for (auto& v : CLUSTERS) {
		Cluster* temp=new Cluster(v);
		temp->FindCentroid();
		clusters.emplace_back(temp);
		
		if (v.size() > max_cluster_size) {
			max_cluster_size = v.size();
		}
	}
	show_cluster(clusters);
	

	START_TIMER(buildCombiTable)
		buildCombiTable(combi_table, best_st_table, beta, gamma, max_cluster_size);
	STOP_TIMER(buildCombiTable, "buildCombiTable()", logfile)

	for (int i = 0; i < combi_table.size(); i++) {
		cout << "index: " << i << " combi_1: " << combi_table[i].combi_1 << " combi_2: " << combi_table[i].combi_2 << endl;
	}

	START_TIMER(clusterToMBFF)
	for (auto& cluster : clusters) {
		cout << "cluster" << endl;
		cout << cluster->getCells().size()<< endl;
		cout << combi_table[cluster->getCells().size()].combi_1 << endl;
		cout << combi_table[cluster->getCells().size()].combi_2 << endl;
		clusterToMBFF(cluster->getCells(), cluster->getPos(), combi_table, MBFF, combi_table[cluster->getCells().size()].combi_1, combi_table[cluster->getCells().size()].combi_2);
		cout << "MBFF size is : " << MBFF.size() << endl;
	}
	STOP_TIMER(clusterToMBFF, "clusterToMBFF()", logfile);
	
	START_TIMER(legal)
		mbffInitialize(MBFF,best_st_table);
		for (int i = 0; i < MBFF.size(); i++) {
			MBFF[i].set_clusterNum(i);
		}
		legalize(MBFF,bin_width, bin_height, die.x_min,die.y_min,die.x_max,die.y_max);
	STOP_TIMER(legal,"legalize()",logfile)
	

	/*
	// 打印結果
	cout << "\n";
	for (auto v : FF) {
		cout << v.get_inst_name() << "'s pos: ";
		for (auto u : v.getPos()) {
			cout << u<<" ";
		}
		cout << "\n";
	}
	cout << "size of clusters = " << clusters.size() << endl;
	count = 0;
	for (auto& cluster : clusters) {
		cout << "cluster " << count << " :\n";
		for (auto& u : cluster->getCells()) {
			cout << "\t" << u->get_inst_name() << " : ";
			for (auto z : u->getPos()) {
				cout << z << " ";
			}
			cout << "\n";
		}
		cout << "\n";
		count++;
	}
	*/
	
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
	while (getline(file, line)) {
		istringstream linestream(line);

		vector<string> tokens;
		string token;
		
		cout << "\tcurrent read to line " << count <<" : "<<line << endl;
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
			standard_Gate.reserve(standard_Gate.capacity() + 1);
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
			standard_Gate.push_back(temp_cell);
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
				for (auto v : standard_FF) {
					if (v.get_ff_name() == tokens[2]) {
						Cell temp_cell = v;
						temp_cell.set_inst(tokens[1], stoi(tokens[3]), stoi(tokens[4]));
						FF.emplace_back(temp_cell);
						find = true;
						break;
					}
				}
				if (!find) {
					for (auto v : standard_Gate) {
						if (v.get_ff_name() == tokens[2]) {
							Cell temp_cell = v;
							temp_cell.set_inst(tokens[1], stoi(tokens[3]), stoi(tokens[4]));
							Gate.emplace_back(temp_cell);
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
							iter.set_timing_slack(stof(tokens[3]));
							cout << "set_timing_slack=" << iter.get_timing_slack()<<endl;
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
					v.set_power(stof(tokens[2]));
					break;
				}
			}
			for (auto &v : FF) {
				if (v.get_ff_name() == tokens[1]) {
					v.set_power(stof(tokens[2]));
				}
			}
		}
		/////////////////////////////////////////////////////////////////
		count++;
		//file.ignore(numeric_limits<streamsize>::max(), '\n');
		fflush(stdin);
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
	
}
void show_cluster(vector<Cluster*> clusters) {
	
	ofstream outFile("show_clusters_after_EMS.txt");
	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}
	outFile << "size of clusters = " << clusters.size() << endl;
	int count = 0;
	for (auto& v : clusters) {
		outFile << "cluster " << count << " :\n";
		for (auto& u : v->getCells()) {
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


#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include <fstream>
#include <sstream>
using namespace std;
#include "cell.h"
void show_stardard_FF(vector<Cell>& input) {
	ofstream outFile("show_stardard_FF.txt");

	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}
	for (auto temp : input) {
		//outFile << "For standard FF" << endl;
		outFile << temp.ff_name << ":" << endl;
		outFile << "\tbit:\t" << temp.bit << endl;
		outFile << "\theight:\t" << temp.ff_height << "\twidth\t" << temp.ff_width << endl;
		outFile << "\tpincout:\t" << temp.pin_count << endl;
		for (auto v : temp.ff_pin) {
			outFile << "\t\tpin:\t" << v.pin_name << ":(" << v.x_pos << "," << v.y_pos << ")" << endl;
		}
		outFile << "\tQpinDelay:\t" << temp.q_pin_delay << endl;
		outFile << "\tgate_power:\t" << temp.gate_power << endl;
	}
	
}
void show_FF(vector<Cell>& input) {
	ofstream outFile("show_FF.txt");
	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}
	for (auto temp : input) {
		//outFile << "For inst FF" << endl;
		outFile << temp.inst_name << ":" << endl;
		outFile << "\tmodel:\t" << temp.ff_name << endl;
		outFile << "\tbit:\t" << temp.bit << endl;
		outFile << "\tx_pox\t" << temp.x_pos << "\ty_pos\t" << temp.y_pos << endl;
		outFile << "\theight:\t" << temp.ff_height << "\twidth\t" << temp.ff_width << endl;
		outFile << "\tpincout:\t" << temp.pin_count << endl;
		for (auto v : temp.ff_pin) {
			outFile << "\t\tpin:\t" << v.pin_name << ":(" << v.x_pos << "," << v.y_pos << ")\tTimingSlack=" << v.timing_slack << endl;
		}
		outFile << "\tQpinDelay:\t" << temp.q_pin_delay << endl;
		outFile << "\tgate_power:\t" << temp.gate_power << endl;
	}
}
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
		outFile << temp.get_ff_name() << ":" << endl;
		outFile << "\tbit:\t" << temp.get_bit() << endl;
		outFile << "\theight:\t" << temp.get_ff_height() << "\twidth\t" << temp.get_ff_width() << endl;
		outFile << "\tpincout:\t" << temp.get_pin_count() << endl;
		for (auto v : temp.get_pin()) {
			outFile << "\t\tpin:\t" << v.get_pin_name() << ":(" << v.get_pin_xpos() << "," << v.get_pin_ypos() << ")" << endl;
		}
		outFile << "\tQpinDelay:\t" << temp.get_q() << endl;
		outFile << "\tgate_power:\t" << temp.get_power() << endl;
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
		outFile << temp.get_inst_name()<< ":" << endl;
		outFile << "\tmodel:\t" << temp.get_ff_name() << endl;
		outFile << "\tbit:\t" << temp.get_bit() << endl;
		outFile << "\tx_pox\t" << temp.get_xpos() << "\ty_pos\t" << temp.get_ypos() << endl;
		outFile << "\theight:\t" << temp.get_ff_height() << "\twidth\t" << temp.get_ff_width() << endl;
		outFile << "\tpincout:\t" << temp.get_pin_count() << endl;
		for (auto v : temp.get_pin()) {
			outFile << "\t\tpin:\t" << v.get_pin_name() << ":(" << v.get_pin_xpos() << "," << v.get_pin_ypos() << ")\tTimingSlack=" << v.get_timing_slack() << endl;
		}
		outFile << "\tQpinDelay:\t" << temp.get_q() << endl;
		outFile << "\tgate_power:\t" << temp.get_power() << endl;
	}
}
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include <fstream>
#include <sstream>
using namespace std;
#include "netlist.h"
#include "cell.h"
void show_netlist(NetList temp) {
	ofstream outFile("show_netlist.txt");

	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}
	outFile << "NetList have:\t" << temp.net_count << endl;
	for (auto v : temp.contain_net) {
		outFile << "\t" << v.net_name << ":\t";
		for (auto u : v.connect_pin) {
			if (u->belong != NULL) {
				outFile << u->belong->inst_name << "/";
			}
			outFile << u->pin_name<<"  ";
		}
		outFile << endl;
	}
}
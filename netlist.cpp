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
	outFile << "NetList have:\t" << temp.get_net_count() << endl;
	for (auto v : temp.get_contain_net()) {
		outFile << "\t" << v->get_net_name() << ":\t";
		for (auto u : v->get_connect_pin()) {
			if (u->get_belong() != NULL) {
				outFile << u->get_belong()->get_inst_name() << "/";
			}
			outFile << u->get_pin_name() << "  ";
		}
		outFile << endl;
	}
}
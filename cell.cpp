#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include <fstream>
#include <sstream>
using namespace std;
#include "cell.h"



void InitialDebanking(vector<Cell>& FF, vector<Cell*>& best_st_table) {
	// �ˬd best_st_table �O�_����
	if (best_st_table.empty()) {
		cerr << "Error: best_st_table is empty!" << endl;
		return;
	}

	// �ǳƦs�x�ݭn���J�� Cell
	vector<Cell> newCells;

	for (int i = 0; i < FF.size(); ++i) {
		cout << "i=" << i << endl;
		if (FF[i].get_bit() > 1) {
			cout << "start to debanking" << endl;
			// �����e��m
			Point currentPos = FF[i].getPos();
			// �ƻs�M���]��m
			Cell original_FF = FF[i];
			
			for (int j = 0; j < original_FF.get_bit(); ++j) {
				Cell copy_cell = *best_st_table[0];
				copy_cell.setPos(currentPos);
				copy_cell.set_inst_name(original_FF.get_inst_name() + "'" + to_string(j));
				copy_cell.get_pin().at(0).set_timing_slack(original_FF.get_pin().at(j).get_timing_slack());
				if (j == 0) {
					FF[i] = copy_cell; // ��s��e Cell
				}
				else {
					newCells.emplace_back(copy_cell); // �K�[��s�V�q
				}
			}
		}
	}

	// �b�M��������@���ʴ��J�s����
	FF.reserve(FF.size() + newCells.size());
	FF.insert(FF.end(), newCells.begin(), newCells.end());
	
}



void show_stardard_FF(vector<Cell>& input) {
	ofstream outFile("show_stardard_FF.txt");

	if (!outFile.is_open()) {
		std::cerr << "�L�k���}�ɮ�" << std::endl;
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
		std::cerr << "�L�k���}�ɮ�" << std::endl;
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
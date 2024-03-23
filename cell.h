#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
#include"pin.h"

#ifndef	_CELL_H_
#define _CELL_H_

class Cell {
public:
	int bit;
	string ff_name;
	int ff_width, ff_height;
	int pin_count;
	vector<Pin> ff_pin;
	int x_pos, y_pos;
	string inst_name;
	
	int q_pin_delay;
	int gate_power;
	Cell(int bit, string ff_name, int ff_width, int ff_height, int pin_count) {
		this->bit = bit;
		this->ff_name = ff_name;
		this->ff_width = ff_width;
		this->ff_height = ff_height;
		this->pin_count = pin_count;
	}
	void set_pin(Pin temp) {
		ff_pin.reserve(ff_pin.capacity() + 1);
		ff_pin.push_back(temp);
	}
	void set_inst(string inst_name,int x_pos,int y_pos) {
		this->inst_name = inst_name;
		this->x_pos = x_pos;
		this->y_pos = y_pos;
	}
	void set_q(int temp) {
		this->q_pin_delay = temp;
	}

};
class Inst {
	string inst_name;
	string contain_ff_name;
	int x_pos, y_pos;
};
void show_stardard_FF(vector<Cell>& input);
void show_FF(vector<Cell>& input);
#endif _CELL_H_
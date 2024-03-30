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
private:
	int bit;
	string ff_name;
	int ff_width, ff_height;
	int pin_count;
	vector<Pin> ff_pin;
	int x_pos, y_pos;
	string inst_name;
	
	int q_pin_delay;
	int gate_power;
public:
	Cell(int bit, string ff_name, int ff_width, int ff_height, int pin_count) {
		this->bit = bit;
		this->ff_name = ff_name;
		this->ff_width = ff_width;
		this->ff_height = ff_height;
		this->pin_count = pin_count;
	}
	void set_inst(string inst_name,int x_pos,int y_pos) {
		this->inst_name = inst_name;
		this->x_pos = x_pos;
		this->y_pos = y_pos;
	}
	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_pin(Pin temp) {
		ff_pin.reserve(ff_pin.capacity() + 1);
		ff_pin.push_back(temp);
	}
	void set_q(int temp) {
		q_pin_delay = temp;
	}
	void set_power(int temp) {
		gate_power=temp;
	}
	////////get////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int get_bit() {
		return bit;
	}
	string get_ff_name() {
		return ff_name;
	}
	int get_ff_width() {
		return ff_width;
	}
	int get_ff_height() {
		return ff_height;
	}
	int get_pin_count() {
		return pin_count;
	}
	vector<Pin>& get_pin() {
		return ff_pin;
	}
	int get_xpos() {
		return x_pos;
	}
	int get_ypos() {
		return y_pos;
	}
	string get_inst_name() {
		return inst_name;
	}
	int get_q() {
		return q_pin_delay;
	}
	int get_power() {
		return gate_power;
	}
	
	
};
/*class Inst {
	string inst_name;
	string contain_ff_name;
	int x_pos, y_pos;
};*/
void show_stardard_FF(vector<Cell>& input);
void show_FF(vector<Cell>& input);
#endif _CELL_H_
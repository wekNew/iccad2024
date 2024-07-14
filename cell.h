#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
#include"pin.h"
#include "Point.h"

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
	Point pos;
	string inst_name;
	int cluster_num;

	int q_pin_delay;
	float gate_power;

	int p_right;//penalty
	int p_left;//penalty
	int p_up;//penalty
	int p_down;//penalty

	vector<Cell*> children;

public:
	Cell(int bit, string ff_name, int ff_width, int ff_height, int pin_count) {
		this->bit = bit;
		this->ff_name = ff_name;
		this->ff_width = ff_width;
		this->ff_height = ff_height;
		this->pin_count = pin_count;
		cluster_num = -1;
		children.clear();
	}
	//·sªººc³y¨ç¼Æ(¥Î¦blegalize¤W­±)
	/*Cell(int bit, int ff_width, int ff_height, int x_pos, int y_pos, int p_right, int p_left, int p_up, int p_down, int cluster_num) {
		this->bit = bit;
		this->ff_width = ff_width;
		this->ff_height = ff_height;
		this->x_pos = x_pos;
		this->y_pos = y_pos;
		this->p_right = p_right;
		this->p_left = p_left;
		this->p_up = p_up;
		this->p_down = p_down;
		this->cluster_num = cluster_num;
		pin_count = 0;
		children.clear();
	}*/
	void set_inst(string inst_name, int x_pos, int y_pos) {
		this->inst_name = inst_name;
		pos = { (float)x_pos,(float)y_pos };
		this->x_pos = x_pos;
		this->y_pos = y_pos;
	}
	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_bit(int bit) {
		this->bit = bit;
	}
	void set_inst_name(string temp) {
		inst_name = temp;
	}
	void set_pin(Pin temp) {
		ff_pin.reserve(ff_pin.capacity() + 1);
		ff_pin.push_back(temp);
	}
	void set_q(int temp) {
		q_pin_delay = temp;
	}
	void setPos(Point newPos)
	{
		pos = newPos;
	}
	void set_power(float temp) {
		gate_power = temp;
	}
	void set_children(vector<Cell*> children) {
		this->children = children;
	}
	void set_clusterNum(int num) {
		cluster_num = num;
	}
	void set_p_right(int penalty) {//penalty
		p_right = penalty;
	}
	void set_p_left(int penalty) {//penalty
		p_left = penalty;
	}
	void set_p_up(int penalty) {//penalty
		p_up = penalty;
	}
	void set_p_down(int penalty) {//penalty
		p_down = penalty;
	}
	void set_ff_name(string name){
		ff_name = name;
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

	Point getPos() {
		return pos;
	}
	Point& get_pos_address() {
		return pos;
	}
	int get_clusterNum() {
		return cluster_num;
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
	float get_power() {
		return gate_power;
	}
	float get_min_slack() {
		float min = 1000;
		for (auto& v : ff_pin) {
			if (v.get_pin_name().at(0) == 'D' && v.get_timing_slack() < min) {
				min = v.get_timing_slack();
			}
		}
		return min;
	}
	vector<Cell*> get_children() {
		return children;
	}
	int get_p_right() {//penalty
		return p_right;
	}
	int get_p_left() {//penalty
		return p_left;
	}
	int get_p_up() {//penalty
		return p_up;
	}
	int get_p_down() {//penalty
		return p_down;
	}
	//////////////////////////////////////////////////////////////////////////
	void change_pin(int index, Pin temp) {
		if (index < ff_pin.size()) {
			ff_pin[index] = temp;
		}
	}
	void initialize_pin() {
		ff_pin.clear();
		pin_count = 0;
	}

};
/*class Inst {
	string inst_name;
	string contain_ff_name;
	int x_pos, y_pos;
};*/
void InitialDebanking(vector<Cell>& FF, vector<Cell*>& best_st_table);
void show_stardard_FF(vector<Cell>& input);
void show_FF(vector<Cell>& input);
#endif 

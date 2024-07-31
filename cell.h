#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include<set>
using namespace std;
#include"pin.h"
#include "Point.h"
#include "netlist.h"

#ifndef	_CELL_H_
#define _CELL_H_

class Cell {
private:
	int bit;
	string ff_name;
	int ff_width, ff_height;
	int pin_count;
	vector<Pin> ff_pin;
	float x_pos, y_pos;
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
	set<int> vertical_overlap_index;
	set<int> horizontal_overlap_index;

public:
	// 默认构造函数
	Cell(int bit, string ff_name, int ff_width, int ff_height, int pin_count)
		: bit(bit), ff_name(ff_name), ff_width(ff_width), ff_height(ff_height),
		pin_count(pin_count), cluster_num(-1), x_pos(0), y_pos(0),
		q_pin_delay(0), gate_power(0.0), p_right(0), p_left(0), p_up(0), p_down(0) {}

	// 复制构造函数
	Cell(const Cell& other)
		: bit(other.bit), ff_name(other.ff_name), ff_width(other.ff_width), ff_height(other.ff_height),
		pin_count(other.pin_count), x_pos(other.x_pos), y_pos(other.y_pos), pos(other.pos),
		inst_name(other.inst_name), cluster_num(other.cluster_num), q_pin_delay(other.q_pin_delay),
		gate_power(other.gate_power), p_right(other.p_right), p_left(other.p_left),
		p_up(other.p_up), p_down(other.p_down) {
		// 深拷贝 ff_pin
		for (const auto& pin : other.ff_pin) {
			ff_pin.push_back(pin); // 使用拷贝构造函数复制 Pin 对象
			ff_pin.back().set_belong(this); // 更新 belong 指针
		}
		children = other.children; // 浅拷贝，因为是指针
		vertical_overlap_index = other.vertical_overlap_index;
		horizontal_overlap_index = other.horizontal_overlap_index;
	}

	// 赋值运算符
	Cell& operator=(const Cell& other) {
		if (this == &other) return *this; // 防止自我赋值

		bit = other.bit;
		ff_name = other.ff_name;
		ff_width = other.ff_width;
		ff_height = other.ff_height;
		pin_count = other.pin_count;
		x_pos = other.x_pos;
		y_pos = other.y_pos;
		pos = other.pos;
		inst_name = other.inst_name;
		cluster_num = other.cluster_num;
		q_pin_delay = other.q_pin_delay;
		gate_power = other.gate_power;
		p_right = other.p_right;
		p_left = other.p_left;
		p_up = other.p_up;
		p_down = other.p_down;

		// 清空当前的 ff_pin 并进行深拷贝
		ff_pin.clear();
		for (const auto& pin : other.ff_pin) {
			ff_pin.push_back(pin); // 使用拷贝构造函数复制 Pin 对象
			ff_pin.back().set_belong(this); // 更新 belong 指针
		}

		children = other.children; // 浅拷贝，因为是指针
		vertical_overlap_index = other.vertical_overlap_index;
		horizontal_overlap_index = other.horizontal_overlap_index;

		return *this;
	}

	void set_inst(string inst_name, float x_pos, float y_pos) {
		this->inst_name = inst_name;
		pos = { x_pos,y_pos };
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
	void set_ff_height(int temp) {
		ff_height = temp;
	}
	void set_ff_width(int temp) {
		ff_width = temp;
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
	void set_ff_name(string name) {
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
	vector<Cell*>& get_children() {
		return children;
	}
	set<int>& get_vertical_overlap_index() {
		return vertical_overlap_index;
	}
	set<int>& get_horizontal_overlap_index() {
		return horizontal_overlap_index;
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
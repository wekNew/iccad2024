#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include<memory>
using namespace std;
//#include "cell.h"
#ifndef	_PIN_H_
#define _PIN_H_
class Cell;
class Net;
class Pin {
private:
	string pin_name;
	int x_pos, y_pos;
	float timing_slack;
	weak_ptr<Cell> belong;
	weak_ptr<Net> clk_net;
	//Cell *belong;
public:
	Pin(string pin_name, int x_pos, int y_pos)
		: pin_name(pin_name), x_pos(x_pos), y_pos(y_pos), timing_slack(0.0f) {}

	Pin(const Pin& other) = default;
	Pin& operator=(const Pin& other) = default;
	Pin(Pin&& other) noexcept = default;
	Pin& operator=(Pin&& other) noexcept = default;


	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_timing_slack(float temp) {
		timing_slack = temp;
	}
	
	void set_belong(shared_ptr<Cell> cell) {
		belong = cell; // 赋值 weak_ptr
	}
	void set_clk_net(shared_ptr<Net> temp) {
		clk_net = temp;
	}
	////////get////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string get_pin_name() {
		return pin_name;
	}
	int get_pin_xpos() {
		return x_pos;
	}
	int get_pin_ypos() {
		return y_pos;
	}
	float get_timing_slack() {
		return timing_slack;
	}
	shared_ptr<Cell> get_belong() {
		return belong.lock(); // 获取强引用，如果 Cell 已被销毁，返回空的 shared_ptr
	}
	shared_ptr<Net> get_clk_net() {
		return clk_net.lock();
	}

};
void show_pin();
#endif 



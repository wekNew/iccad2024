#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
//#include "cell.h"
#ifndef	_PIN_H_
#define _PIN_H_
class Cell;
class Pin {
private:
	string pin_name;
	int x_pos, y_pos;
	float timing_slack;
	Cell *belong;
public:
	Pin(string pin_name, int x_pos, int y_pos) {
		this->pin_name = pin_name;
		this->belong = NULL;
		this->x_pos = x_pos;
		this->y_pos = y_pos;
	}
	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_timing_slack(float temp) {
		timing_slack = temp;
	}
	
	void set_belong(Cell* temp) {
		belong = temp;
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
	Cell* get_belong() {
		return belong;
	}

};
void show_pin();
#endif _PIN_H_



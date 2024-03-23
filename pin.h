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
public:
	string pin_name;
	int x_pos, y_pos;
	int timing_slack;
	Cell *belong;

	Pin(string pin_name, int x_pos, int y_pos) {
		this->pin_name = pin_name;
		this->belong = NULL;
		this->x_pos = x_pos;
		this->y_pos = y_pos;
	}

};
void show_pin();
#endif _PIN_H_



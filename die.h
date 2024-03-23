#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
#ifndef	_DIE_H_
#define _DIE_H_

struct Die {
	int x_min, y_min, x_max, y_max;
	int input_pin_num, optput_pin_num;
	vector<Pin> input_pin, output_pin;
};
void show_die();
#endif _DIE_H_
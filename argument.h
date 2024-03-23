#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
#ifndef	_ARGUMENT_H_
#define _ARGUMENT_H_

	extern int alpha, beta, gamma, delta;
	extern int bin_width, bin_height, bin_max_util;
	extern int row_start_x, row_start_y, row_width, row_height;
	extern float displacement_delay;

	void show_argument();
#endif _ARGUMENT_H_
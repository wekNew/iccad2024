#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include <fstream>
#include <sstream>
using namespace std;
#include"argument.h"

void show_argument() {
	ofstream outFile("show_argument.txt");

	if (!outFile.is_open()) {
		std::cerr << "無法打開檔案" << std::endl;
		return;
	}
	
	outFile << "Alpha  " << alpha << endl;
	outFile << "Beta  " << beta << endl;
	outFile << "Gamma  " << gamma << endl;
	outFile << "Delta  " << delta << endl;
	outFile << "Bin_width  " << bin_width << endl;
	outFile << "Bin_height  " << bin_height << endl;
	outFile << "Bin_max_util  " << bin_max_util << endl;
	outFile << "Row_start_x  " << row_start_x << endl;
	outFile << "Row_start_y  " << row_start_y << endl;
	outFile << "Row_width  " << row_width << endl;
	outFile << "Row_height  " << row_height << endl;
	outFile << "Displacement_delay  " << displacement_delay << endl;

	
	outFile.close();
	return;
}
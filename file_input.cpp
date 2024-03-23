using namespace std;
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include <fstream>
#include <sstream>

#include"basic_data.h"
#include"function.h"
#include"object.h"

string input_filename;
void file_input(){
	ifstream file(input_filename);

	if (!file.is_open()) {
		cout << "無法打開檔案" << endl;
		return;
	}
	string line;
	//int L = 0;
	while (getline(file, line)) {
		istringstream linestream(line);

		vector<string> tokens;
		string token;
		while (linestream >> token) {
			tokens.push_back(token);
		}
		/////////////////////////////////////////////////////////////////
		if (tokens[0] == "Alpha") {
			environment_coeff.alpha = stoi(tokens[1]);
		}
		else if (tokens[0] == "Beta") {
			environment_coeff.beta = stoi(tokens[1]);
		}
		else if (tokens[0] == "Gamma") {
			environment_coeff.gamma = stoi(tokens[1]);
		}
		else if (tokens[0] == "Delta") {
			environment_coeff.delta = stoi(tokens[1]);
		}
		else if (tokens[0] == "DieSize") {
			environment_coeff.delta = stoi(tokens[1]);
			die.x_min = stoi(tokens[1]);
			die.y_min = stoi(tokens[2]);
			die.x_max = stoi(tokens[3]);
			die.y_max = stoi(tokens[4]);
		}
		/////////////////////////////////////////////////////////////////
		if (tokens[0] == "NumInput") {

		}
	}
	file.close();
	return;
}
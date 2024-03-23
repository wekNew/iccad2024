#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
#include "Pin.h"
#ifndef	_NETLIST_H_
#define _NETLIST_H_

class Net {
public:
	string net_name;
	int net_count;
	vector<Pin*> connect_pin;
	Net(string net_name,int net_count) {
		this->net_name = net_name;
		this->net_count = net_count;
	}
	void set_pin(Pin* temp) {
		connect_pin.reserve(connect_pin.capacity() + 1);
		connect_pin.push_back(temp);
	}

};
class NetList {
public:
	int net_count;
	vector<Net> contain_net;
	void set_net(Net temp) {
		contain_net.reserve(contain_net.capacity() + 1);
		contain_net.push_back(temp);
	}
};
void show_netlist(NetList temp);
#endif _NETLIST_H_
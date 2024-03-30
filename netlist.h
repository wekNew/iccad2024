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
private:
	string net_name;
	int pin_count;
	vector<Pin*> connect_pin;
public:
	Net(string net_name,int pin_count) {
		this->net_name = net_name;
		this->pin_count = pin_count;
	}
	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_pin_count(int temp) {
		pin_count = temp;
	}
	void set_pin(Pin* temp) {
		connect_pin.reserve(connect_pin.capacity() + 1);
		connect_pin.push_back(temp);
	}
	////////get////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string get_net_name() {
		return net_name;
	}
	int get_pin_count() {
		return pin_count;
	}
	vector<Pin*>& get_connect_pin() {
		return connect_pin;
	}
};
class NetList {
private:
	int net_count;
	vector<Net> contain_net;
public:
	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_net_count(int temp) {
		net_count = temp;
	}
	void set_net(Net temp) {
		contain_net.reserve(contain_net.capacity() + 1);
		contain_net.push_back(temp);
	}
	////////get////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int get_net_count() {
		return net_count;
	}
	vector<Net>& get_contain_net() {
		return contain_net;
	}
};
void show_netlist(NetList temp);
#endif _NETLIST_H_
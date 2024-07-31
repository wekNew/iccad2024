#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
#include<memory>
using namespace std;
#include "pin.h"
#ifndef	_NETLIST_H_
#define _NETLIST_H_

class Net {
private:
	string net_name;
	int pin_count;
	//vector<Pin*> connect_pin;
	vector<shared_ptr<Pin>> connect_pin;
public:
	Net(string net_name, int pin_count)
		: net_name(net_name), pin_count(pin_count) {}

	Net(const Net& other) = default;
	Net& operator=(const Net& other) = default;
	Net(Net&& other) noexcept = default;
	Net& operator=(Net&& other) noexcept = default;

	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_pin_count(int temp) {
		pin_count = temp;
	}
	void add_pin(shared_ptr<Pin> pin) {
		connect_pin.emplace_back(pin); // ²K¥[ shared_ptr
	}
	////////get////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string get_net_name() {
		return net_name;
	}
	int get_pin_count() {
		return pin_count;
	}
	vector<shared_ptr<Pin>>& get_connect_pin() {
		return connect_pin;
	}
};
class NetList {
private:
	int net_count=0;
	vector<shared_ptr<Net>> contain_net;
public:
	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_net_count(int temp) {
		net_count = temp;
	}
	void set_net(shared_ptr<Net> temp) {
		//contain_net.reserve(contain_net.capacity() + 1);
		contain_net.emplace_back(temp);
	}
	////////get////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int get_net_count() {
		return net_count;
	}
	vector<shared_ptr<Net>>& get_contain_net() {
		return contain_net;
	}
};
void show_netlist(NetList temp);
#endif 
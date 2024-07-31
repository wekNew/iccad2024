#pragma once
#include <stdio.h>
#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
#include "pin.h"
#ifndef	_NETLIST_H_
#define _NETLIST_H_

class Net {
private:
	string net_name;
	int pin_count;
	vector<Pin*> connect_pin;
public:
	Net(string net_name, int pin_count) : net_name(net_name), pin_count(pin_count) {}

	// 复制构造函数
	Net(const Net& other) : net_name(other.net_name), pin_count(other.pin_count) {
		// 深拷贝 connect_pin
		for (const auto& pin : other.connect_pin) {
			connect_pin.push_back(pin); // 只拷贝指针，指向原始的 Pin 对象
		}
	}

	// 赋值运算符
	Net& operator=(const Net& other) {
		if (this == &other) return *this; // 防止自我赋值

		net_name = other.net_name;
		pin_count = other.pin_count;

		// 清空当前的 connect_pin，并进行深拷贝
		connect_pin.clear();
		for (const auto& pin : other.connect_pin) {
			connect_pin.push_back(pin); // 只拷贝指针，指向原始的 Pin 对象
		}

		return *this;
	}

	~Net() {
		// 不需要删除 connect_pin 中的 Pin 对象，因为它们的生命周期由 Cell 管理
	}
	////////set////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_pin_count(int temp) {
		pin_count = temp;
	}
	void set_pin(Pin* temp) {
		//connect_pin.reserve(connect_pin.capacity() + 1);
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
	void add_net(Net temp) {
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
#endif 
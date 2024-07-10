#include "perturbation.h"



void perturbation(vector<Cell>& FF, vector<Cell> standard_FF,int max_population,float mutation_ratio,int max_iteration) {
	srand(time(NULL));
	if (max_population % 2 != 0) {
		max_population + 1;
	}
	CostFF ancestor = {FF,0};
	vector<CostFF> offspring_FF(max_population, ancestor);

	int max_bit = standard_FF[standard_FF.size() - 1].get_bit();
	vector<int> standard_bit_index(max_bit+1,0);

	for (int i = 1; i < standard_FF.size(); ++i) {
		if (standard_FF[i-1].get_bit() != standard_FF[i].get_bit()) {
			standard_bit_index[standard_FF[i-1].get_bit()]=i;
		}
	}
	standard_bit_index[max_bit] = standard_FF.size() + 1;
	for (int i = 0; i < max_population; ++i) {//初始突變
		
		for (int j = 0; j < FF.size() * mutation_ratio; ++j) {
			int mutate_index =(i%2==0)? rand() % (FF.size() / 2): (rand() % (FF.size() / 2)+FF.size()/2)+1;
			ChangeSameBitCell(offspring_FF[i].cells.at(mutate_index), standard_FF, standard_bit_index);
		}
	}
	while (max_iteration-->0) {
		vector<CostFF> new_children;
		for (int i = 0; i < max_population; i++) {
			vector<Cell> temp;
			temp.reserve(FF.size());
			size_t mid1 = offspring_FF[i].cells.size() / 2;
			if (i % 2 == 0) {
				temp.insert(temp.end(), offspring_FF[i].cells.begin(), offspring_FF[i].cells.begin()+mid1);
				temp.insert(temp.end(), offspring_FF[i+1].cells.begin()+mid1, offspring_FF[i+1].cells.begin());
			}
			else {
				temp.insert(temp.end(), offspring_FF[i].cells.begin(), offspring_FF[i].cells.begin() + mid1);
				temp.insert(temp.end(), offspring_FF[i - 1].cells.begin() + mid1, offspring_FF[i - 1].cells.begin());
			}
			CostFF Temp1 = { temp,0 };
			/*Legalize(temp);
			* Temp1.cost=cost(temp);
			
			*/
			new_children.emplace_back(Temp1);
		}
		for (int i = 0; i < mutation_ratio * offspring_FF.size() * max_bit; ++i) {
			vector<Cell> temp=FF;
			for (int j = 0; j < FF.size() * mutation_ratio; ++j) {
				int mutate_index = rand() % (FF.size());
				ChangeSameBitCell(temp.at(mutate_index), standard_FF, standard_bit_index);
			}
			CostFF Temp2 = { temp,0 };
			//Temp2.cost=cost(temp);
			new_children.emplace_back(Temp2);
		}

		offspring_FF.insert(offspring_FF.end(), new_children.begin(), new_children.end());
		std::sort(offspring_FF.begin(), offspring_FF.end(), [](const CostFF& a, const CostFF& b) {
			return a.cost < b.cost;
			});
		offspring_FF.resize(max_population);

	}
	FF = offspring_FF[0].cells;
	
}
void ChangeSameBitCell(Cell& target_cell, vector<Cell> standard_FF, vector<int> standard_bit_index) {
	int target_bit = target_cell.get_bit();
	if (target_bit < 1) {
		cout << "this cell's bit <1\n";
		return;
	}
	int new_cell_index = rand() % (standard_bit_index[target_bit] - standard_bit_index[target_bit - 1]) + standard_bit_index[target_bit - 1];
	Cell copy_cell = standard_FF[new_cell_index];
	copy_cell.setPos(target_cell.getPos());
	copy_cell.set_children(target_cell.get_children());
	copy_cell.set_inst(target_cell.get_inst_name(), target_cell.get_xpos(), target_cell.get_ypos());
	for (int i = 0; i < copy_cell.get_pin_count(); ++i) {//更新cell中每個pin的timing_slack
		for (int j = 0; j < target_cell.get_pin_count(); ++j) {
			if (copy_cell.get_pin().at(i).get_pin_name() == target_cell.get_pin().at(j).get_pin_name()) {
				copy_cell.get_pin().at(i).set_timing_slack(target_cell.get_pin().at(j).get_timing_slack());
			}
		}
	}
	target_cell = copy_cell;
}
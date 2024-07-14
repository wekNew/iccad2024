#include "table.h"

using namespace std;


void buildCombiTable(vector<combination>& combi_table, vector<Cell*>& best_st_table, int beta, int gamma, int max_cluster_size) {
	combi_table.resize(max_cluster_size + 1);
	for (int i = 1; i <= max_cluster_size; i++) {
		for (auto st : best_st_table) {
			int st_bit = st->get_bit();
			int st_power = st->get_power();
			int st_area = st->get_ff_height() * st->get_ff_width();
			int st_cost = beta * st_power + gamma * st_area;

			if (st_bit < i) {
				int q = i - st_bit;
				if ((st_cost + combi_table[q].cost) < combi_table[i].cost) {
					combi_table[i].cost = (st_cost + combi_table[q].cost);
					combi_table[i].combi_1 = st_bit;
					combi_table[i].combi_2 = q;
				}
			}
			else {
				if ((beta * st_power + gamma * st_area) < combi_table[i].cost) {
					combi_table[i].cost = st_cost;
					combi_table[i].combi_1 = st_bit;
					combi_table[i].combi_2 = -1;
				}
			}
		}
	}
}
void buildBestStTable(vector<Cell>& standard_FF, vector<Cell*>& best_st_table, int beta, int gamma) {

	for (auto& st : standard_FF) {
		int cost = INT_MAX;
		int st_bit = st.get_bit();
		int st_power = st.get_power();
		int st_area = st.get_ff_height() * st.get_ff_width();

		//best_st_table
		bool is_find = false;
		for (auto& best_st : best_st_table) {
			if (best_st->get_bit() == st_bit) {
				is_find = true;
				if ((beta * st_power + gamma * st_area) < (beta * best_st->get_power() + gamma * best_st->get_ff_width() * best_st->get_ff_height())) {
					best_st = &st;
				}
			}
		}
		if (is_find == false) {
			best_st_table.push_back(&st);
		}
	}


	for (int i = 0; i < best_st_table.size(); i++) {
		cout << best_st_table[i]->get_bit() << " " << best_st_table[i]->get_ff_name() << endl;
	}


}
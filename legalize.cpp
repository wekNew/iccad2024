#include <iostream>
#include<cmath>
#include<cstdlib>
#include<vector>
#include<queue>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "legalize.h"
#include"Cell.h"
#include"Point.h"
#include"Partition.h"

using namespace std;

int BinMaxUtil = 75;
int binwidth = 0;
int binheight = 0;
int Die_max_x = 0;
int Die_max_y = 0;
int Die_min_x = 0;
int Die_min_y = 0;
int max_penalty = 10;

struct Event {
	int x;//x或y的座標
	int cluster_idx;//第幾個cluster
	int is_start;//表示是開始(左邊)還是結束(右邊)

	bool operator<(const Event& other) const {
		if (x != other.x)
			return x < other.x;
		return is_start && !other.is_start;
	}
};



void to_the_site(int& x, int& y) {//到site上
	int new_x = x / binwidth;
	int new_y = y / binheight;
	x = binwidth * new_x;
	y = binheight * new_y;
	cout << ",\tafter shift,\tx:" << x << ",\ty:" << y;
}


//印出graph
void print_overlap_graph(const unordered_map<int, unordered_set<int>>& overlap_graph) {
	for (const auto& entry : overlap_graph) {
		cout << "Cluster " << entry.first << " overlaps with: ";
		for (int neighbor : entry.second) {
			cout << neighbor << " ";
		}
		cout << "\n";
	}
}
//印出edge
void print_edges(const set<pair<int, int>>& edges) {
	cout << "Edges: { ";
	for (const auto& edge : edges) {
		cout << "{" << edge.first << ", " << edge.second << "} ";
	}
	cout << "}\n\n";
}
////找overlap的cluster(vertical&horizontal的interval去找overlap，再建立graph)////
void find_vertical_overlaps(vector<Cell>& MBFF, unordered_map<int, unordered_set<int>>& overlap_graph) {
	vector<Event> events;
	for (int i = 0; i < MBFF.size(); ++i) {
		int y = MBFF[i].getPos().access_Values().at(1);
		int height = MBFF[i].get_ff_height();
		events.push_back({ y, i, true });
		events.push_back({ y + height, i, false });
	}

	sort(events.begin(), events.end());

	set<int> active_clusters;

	for (const auto& event : events) {
		if (event.is_start) {
			for (int active_idx : active_clusters) {
				int active_end_y = MBFF[active_idx].getPos().access_Values().at(1) + MBFF[active_idx].get_ff_height();
				int event_end_y = MBFF[event.cluster_idx].getPos().access_Values().at(1) + MBFF[event.cluster_idx].get_ff_height();
				if (MBFF[event.cluster_idx].getPos().access_Values().at(1) < active_end_y && MBFF[active_idx].getPos().access_Values().at(1) < event_end_y) {
					overlap_graph[MBFF[active_idx].get_clusterNum()].insert(MBFF[event.cluster_idx].get_clusterNum());
					overlap_graph[MBFF[event.cluster_idx].get_clusterNum()].insert(MBFF[active_idx].get_clusterNum());
				}
			}
			active_clusters.insert(event.cluster_idx);
		}
		else {
			active_clusters.erase(event.cluster_idx);
		}
	}
}

void find_horizontal_overlaps(vector<Cell>& MBFF, unordered_map<int, unordered_set<int>>& overlap_graph) {
	vector<Event> events;
	for (int i = 0; i < MBFF.size(); ++i) {
		int x = MBFF[i].getPos().access_Values().at(0);
		int width = MBFF[i].get_ff_width();
		events.push_back({ x, i, true });
		events.push_back({ x + width, i, false });
	}

	sort(events.begin(), events.end());

	set<int> active_clusters;
	for (const auto& event : events) {
		if (event.is_start) {
			for (int active_idx : active_clusters) {
				int active_end_x = MBFF[active_idx].getPos().access_Values().at(0) + MBFF[active_idx].get_ff_width();
				int event_end_x = MBFF[event.cluster_idx].getPos().access_Values().at(0) + MBFF[event.cluster_idx].get_ff_width();
				if (MBFF[event.cluster_idx].getPos().access_Values().at(0) < active_end_x && MBFF[active_idx].getPos().access_Values().at(0) < event_end_x) {
					cout << "C" << MBFF[active_idx].get_clusterNum() << "&C" << MBFF[event.cluster_idx].get_clusterNum() << " overlap\n";
					overlap_graph[MBFF[active_idx].get_clusterNum()].insert(MBFF[event.cluster_idx].get_clusterNum());
					overlap_graph[MBFF[event.cluster_idx].get_clusterNum()].insert(MBFF[active_idx].get_clusterNum());
				}
			}
			active_clusters.insert(event.cluster_idx);
		}
		else {
			active_clusters.erase(event.cluster_idx);
		}
	}
}

set<pair<int, int>> vertical_overlaps_edges;
set<pair<int, int>> horizontal_overlaps_edges;
void creat_overlap_edge(unordered_map<int, unordered_set<int>>& graph, int direction) {//建立vertical(1)和horizontal(2) overlap graph
	for (const auto& entry : graph) {
		int node = entry.first;
		const unordered_set<int>& neighbors = entry.second;
		if (direction == 1) {
			for (int neighbor : neighbors) {
				if (node < neighbor) {//避免 {a,b}& {b,a}
					vertical_overlaps_edges.insert({ node, neighbor });
				}
				else {
					vertical_overlaps_edges.insert({ neighbor, node });
				}
			}
		}
		else if (direction == 2) {
			for (int neighbor : neighbors) {
				if (node < neighbor) {
					horizontal_overlaps_edges.insert({ node, neighbor });
				}
				else {
					horizontal_overlaps_edges.insert({ neighbor, node });
				}
			}
		}

	}

}
//找vertical和horizontal overlap graph 共同的edge	
//vector<cluster, cluster>overlap_edge;//overlap_edges是<int,int>不方便後面移動的部分
void merge_horizontal_vertical_overlap(set<pair<int, int>>& edges1, set<pair<int, int>>& edges2, set<pair<int, int>>& overlap_edges, vector<pair<Cell, Cell>>& need_move_overlap_edges, set<int>& overlap_cluster, vector<Cell>& MBFF) {
	for (const auto& edge1 : edges1) {
		for (const auto& edge2 : edges2) {
			if ((edge1.first == edge2.first && edge1.second == edge2.second) || (edge1.first == edge2.second && edge1.second == edge2.first)) {
				cout << "overlap edge: " << edge1.first << ", " << edge1.second << "\n";//檢查overlap graph
				need_move_overlap_edges.push_back({ MBFF[edge1.first], MBFF[edge1.second] });  // 插入 pair<cell, cell>
				overlap_edges.insert(edge1);
				break;
			}
		}
	}
	for (const auto& edge1 : overlap_edges) {//overlap_edges是vector<int>的資料結構
		overlap_cluster.insert(edge1.first);
		overlap_cluster.insert(edge1.second);
	}
	cout << "finish merge_overlaps\n";
}

bool check_overlap(Cell& cluster1, Cell& cluster2) {
	int cluster1_x_start = cluster1.getPos().access_Values().at(0);
	int cluster1_x_end = cluster1.getPos().access_Values().at(0) + cluster1.get_ff_width();
	int cluster1_y_start = cluster1.getPos().access_Values().at(1);
	int cluster1_y_end = cluster1.getPos().access_Values().at(1) + cluster1.get_ff_height();

	int cluster2_x_start = cluster2.getPos().access_Values().at(0);
	int cluster2_x_end = cluster2.getPos().access_Values().at(0) + cluster2.get_ff_width();
	int cluster2_y_start = cluster2.getPos().access_Values().at(1);
	int cluster2_y_end = cluster2.getPos().access_Values().at(1) + cluster2.get_ff_height();

	// 看x, y方向有沒有overlap
	bool overlap_x = (cluster1_x_start < cluster2_x_end) && (cluster2_x_start < cluster1_x_end);
	bool overlap_y = (cluster1_y_start < cluster2_y_end) && (cluster2_y_start < cluster1_y_end);

	return overlap_x && overlap_y;
}

vector<Cell> find_nearest_overlap_cluster(Cell& current_cluster, vector<Cell> overlap_clusters) {
	vector<Cell>nearest_overlap_cluster;
	double nearest_distance = 1000000000000000.0;//一個極大值
	for (int i = 0; i < overlap_clusters.size(); i++) {
		Cell& cluster = overlap_clusters[i];
		if (fabs(cluster.getPos().access_Values().at(0) + cluster.get_ff_width() - current_cluster.getPos().access_Values().at(0) - current_cluster.get_ff_width()) < nearest_distance && current_cluster.get_clusterNum() != cluster.get_clusterNum()) {
			if (check_overlap(cluster, current_cluster)) {//確認是否有overlap
				nearest_distance = fabs(cluster.getPos().access_Values().at(0) + cluster.get_ff_width() - current_cluster.getPos().access_Values().at(0) - current_cluster.get_ff_width());
				nearest_overlap_cluster.clear(); // 清除之前的所有元素
				nearest_overlap_cluster.push_back(cluster); //添加最接近的overlap cluster
			}
		}
	}
	return nearest_overlap_cluster;
}
bool samewidth(Cell& cluster1, Cell& cluster2) {
	int cluster1_x_start = cluster1.getPos().access_Values().at(0);
	int cluster1_x_end = cluster1.getPos().access_Values().at(0) + cluster1.get_ff_width();
	int cluster2_x_start = cluster2.getPos().access_Values().at(0);
	int cluster2_x_end = cluster2.getPos().access_Values().at(0) + cluster2.get_ff_width();
	return(cluster1_x_start == cluster2_x_start) && (cluster2_x_end == cluster1_x_end);
}
int completely_overlap_move = 0;//完全重疊的次數
void move_cluster(Cell& current_cluster, const vector<Cell>& overlap_clusters, vector<Cell>& MBFF) {
	int unit_width = static_cast<int>(binwidth);
	int unit_height = static_cast<int>(binheight);
	vector<Cell> nearest_overlap = find_nearest_overlap_cluster(current_cluster, overlap_clusters);
	int x = current_cluster.getPos().access_Values().at(0);
	int y = current_cluster.getPos().access_Values().at(1);
	int p_right = current_cluster.get_p_right();
	int p_left = current_cluster.get_p_left();
	int p_up = current_cluster.get_p_up();
	int p_down = current_cluster.get_p_down();

	if (!nearest_overlap.empty()) {
		Cell& nearest_cluster = nearest_overlap[0]; // 提取第一個元素
		int n_p_right = nearest_cluster.get_p_right();
		int n_p_left = nearest_cluster.get_p_left();
		int n_p_up = nearest_cluster.get_p_up();
		int n_p_down = nearest_cluster.get_p_down();
		cout << "current cluster C" << current_cluster.get_clusterNum() << " nearest cluster:" << "C" << nearest_cluster.get_clusterNum() << "\n";
		if (!samewidth(nearest_cluster, current_cluster) && fabs(nearest_cluster.getPos().access_Values().at(0) + nearest_cluster.get_ff_width() - current_cluster.getPos().access_Values().at(0) - current_cluster.get_ff_width()) < fabs(nearest_cluster.getPos().access_Values().at(1) + nearest_cluster.get_ff_height() - current_cluster.getPos().access_Values().at(1) - current_cluster.get_ff_height()) && (n_p_left < max_penalty || n_p_right < max_penalty)) { // 确定水平或垂直移动
			cout << "horizontal move\n";
			if (nearest_cluster.getPos().access_Values().at(0) + nearest_cluster.get_ff_width() < current_cluster.getPos().access_Values().at(0) + current_cluster.get_ff_width() && n_p_right < max_penalty) { // 向右移动
				x = current_cluster.getPos().access_Values().at(0) + unit_width;//改x座標這邊也要改
				cout << "C" << current_cluster.get_clusterNum() << " move right: " << current_cluster.getPos().access_Values().at(0) << "\n";
				p_right++;
				if (current_cluster.getPos().access_Values().at(0) > Die_max_x || current_cluster.getPos().access_Values().at(0) + current_cluster.get_ff_width() > Die_max_x) {
					x = current_cluster.getPos().access_Values().at(0) - unit_width;
					cout << "C" << current_cluster.get_clusterNum() << " 到最右邊界\n";
				}
				//cout << "move right " << current_cluster.p_right << " times\n";確認移動幾次
				if (p_right >= max_penalty)
					cout << "C" << current_cluster.get_clusterNum() << " can not move right!!!\n";//penalty太大不能再往右
			}
			else if (nearest_cluster.getPos().access_Values().at(0) + nearest_cluster.get_ff_width() > current_cluster.getPos().access_Values().at(0) + current_cluster.get_ff_width() && n_p_left < max_penalty) { // 向左移动
				x = current_cluster.getPos().access_Values().at(0) - unit_width;
				cout << "C" << current_cluster.get_clusterNum() << " move left: " << current_cluster.getPos().access_Values().at(0) << "\n";
				p_left++;
				if (current_cluster.getPos().access_Values().at(0) < Die_min_x || current_cluster.getPos().access_Values().at(0) + current_cluster.get_ff_width() < Die_min_x) {
					x = current_cluster.getPos().access_Values().at(0) + unit_width;
					cout << "C" << current_cluster.get_clusterNum() << " 到最左邊界:";
				}
				//cout << "move left " << current_cluster.p_left << " times\n";
				if (p_left >= max_penalty)
					cout << "C" << current_cluster.get_clusterNum() << " can not move left!!!\n";//penalty太大不能再往左
			}
		}
		else { // 垂直移动		
			if (nearest_cluster.getPos().access_Values().at(1) > current_cluster.getPos().access_Values().at(1)) { // 向上移动
				cout << "C" << current_cluster.get_clusterNum() << " move up\n";
				y = current_cluster.getPos().access_Values().at(1) + unit_height;
				p_up++;
				if (current_cluster.getPos().access_Values().at(1) > Die_max_y || current_cluster.getPos().access_Values().at(1) + current_cluster.get_ff_height() > Die_max_y) {
					y = current_cluster.getPos().access_Values().at(1) - unit_height;
					cout << "C" << current_cluster.get_clusterNum() << " 到最下邊界\n";
				}
				if (p_up >= max_penalty)
					cout << "C" << current_cluster.get_clusterNum() << " can not move up!!!\n";//penalty太大不能再往上
			}
			else if (nearest_cluster.getPos().access_Values().at(1) < current_cluster.getPos().access_Values().at(1)) { // 向下移动
				cout << "C" << current_cluster.get_clusterNum() << " move down\n";
				y = current_cluster.getPos().access_Values().at(1) - unit_height;
				p_down++;
				if (current_cluster.getPos().access_Values().at(1) < Die_min_y || current_cluster.getPos().access_Values().at(1) + current_cluster.get_ff_height() < Die_min_y) {
					y = current_cluster.getPos().access_Values().at(1) + unit_height;
					cout << "C" << current_cluster.get_clusterNum() << " 到最下邊界\n";
				}
				if (p_down >= max_penalty)
					cout << "C" << current_cluster.get_clusterNum() << " can not move down!!!\n";//penalty太大不能再往下
			}
			else if (nearest_cluster.getPos().access_Values().at(1) == current_cluster.getPos().access_Values().at(1)) { // 完全重疊
				cout << "C" << current_cluster.get_clusterNum() << " completely overlap\n";
				x = current_cluster.getPos().access_Values().at(0) + unit_width * completely_overlap_move;//第一個留在原地其他往旁邊移
				completely_overlap_move++;
			}
		}
	}
	else {
		cout << "No nearest overlap cluster found for C" << current_cluster.get_clusterNum() << "\n";
	}
	Point new_position({ static_cast<float>(x), static_cast<float>(y) });
	//current_cluster.set_xpos(x);
	//current_cluster.set_ypos(y);
	current_cluster.setPos(new_position);
	current_cluster.set_p_right(p_right);
	current_cluster.set_p_left(p_left);
	current_cluster.set_p_up(p_up);
	current_cluster.set_p_down(p_down);
	for (auto& cluster : MBFF) {   //把current_cluster寫進MBFF
		if (cluster.get_clusterNum() == current_cluster.get_clusterNum()) {
			cluster = current_cluster;
			break;
		}
	}
}
bool slack_bigger(Cell& cluster1, Cell& cluster2) {
	/*
	float slack1 = 0.0;
	for (int i = 0; i < cluster1.get_children().size(); i++) {
		Cell* child = cluster1.get_children()[i];
		for (auto& pin : child->get_pin()) {
			slack1 += pin.get_timing_slack();
		}
	}

	float slack2 = 0.0;
	for (int i = 0; i < cluster2.get_children().size(); i++) {
		Cell* child = cluster2.get_children()[i];
		for (auto& pin : child->get_pin()) {
			slack2 += pin.get_timing_slack();
		}
	}

	return slack1 > slack2;
	*/
	return true;
}

int shift_time = 0;//確認移動幾次而已
queue<Cell> get_overlap_clusters(vector<Cell>& MBFF, vector<pair<Cell, Cell>>& need_move_overlap_edges, vector<Cell>& overlap_clusters) {
	unordered_map<int, unordered_set<int>> overlap_horizontal_graph;
	unordered_map<int, unordered_set<int>> overlap_vertical_graph;

	queue<Cell>overlap_queue;
	find_horizontal_overlaps(MBFF, overlap_horizontal_graph);
	find_vertical_overlaps(MBFF, overlap_vertical_graph);
	vertical_overlaps_edges.clear();
	horizontal_overlaps_edges.clear();

	cout << "finish find_horizontal_overlaps、find_vertical_overlaps\n";

	creat_overlap_edge(overlap_vertical_graph, 1);//1:vettical,2:horizontal
	creat_overlap_edge(overlap_horizontal_graph, 2);

	cout << "finish overlap_edge\n";

	set<pair<int, int>> overlap_edges;
	set<int> overlap_cluster;

	merge_horizontal_vertical_overlap(vertical_overlaps_edges, horizontal_overlaps_edges, overlap_edges, need_move_overlap_edges, overlap_cluster, MBFF);
	if (overlap_cluster.size() < 1) {//完全legal!!!
		cout << "\n///////////congratulation///////////\n";
		cout << "\nvertical: \n";
		print_edges(vertical_overlaps_edges);
		cout << "horizontal:\n";
		print_edges(horizontal_overlaps_edges);
		cout << "Overlap clusters: {\n";
		for (auto& cluster_idx : overlap_cluster) {
			Cell cluster = MBFF[cluster_idx];
			int x = cluster.getPos().access_Values().at(0);
			int y = cluster.getPos().access_Values().at(1);
			cout << "C" << cluster_idx << "=>" << " start_x:" << x << " ,start_y:" << y << " " << "end_x:" << x + cluster.get_ff_width() << " ,end_y:" << y + cluster.get_ff_height() << "\n";
		}//width和height還要解決
		cout << "}\n";
		cout << "!!!legal!!!\n";
		return overlap_queue;
	}
	cout << "\nvertical: \n";
	print_edges(vertical_overlaps_edges);
	cout << "horizontal:\n";
	print_edges(horizontal_overlaps_edges);
	cout << "Overlap clusters: {\n";
	for (auto& cluster_idx : overlap_cluster) {//overlap_cluster是vector<int>的資料結構
		Cell cluster = MBFF[cluster_idx];
		int x = cluster.getPos().access_Values().at(0);
		int y = cluster.getPos().access_Values().at(1);
		cout << "C" << cluster_idx << "=>" << " start_x:" << x << " ,start_y:" << y << " " << "end_x:" << x + cluster.get_ff_width() << " ,end_y:" << y + cluster.get_ff_height() << "\n";
	}
	cout << "}\n";
	for (int cluster_idx : overlap_cluster) {
		overlap_clusters.push_back(MBFF[cluster_idx]);
	}
	auto poriority_bigger = [](Cell& cluster1, Cell& cluster2) {//先比平均的slack再比bit
		if (slack_bigger(cluster1, cluster2) == 1) {
			return true;
		}
		else {
			return cluster1.get_bit() > cluster2.get_bit();
		}
		};
	sort(overlap_clusters.begin(), overlap_clusters.end(), poriority_bigger);//依照poriority先去排我的overlap clusters
	for (auto& cl : overlap_clusters) {//把cluster轉換成queue
		overlap_queue.push(cl);
	}
	cout << "\nafter " << shift_time << " time shift:\n";
	shift_time++;
	cout << "overlap clusters: ";
	for (auto& cluster : overlap_clusters) {
		cout << "C" << cluster.get_clusterNum() << " ";
	}
	cout << "\n\n";
	return overlap_queue;
}

void shift_until_legal(vector<Cell> overlap_clusters, queue<Cell>& overlap_queue, vector<Cell>& MBFF, vector<pair<Cell, Cell>>& need_move_overlap_edges) {
	while (!overlap_queue.empty()) {
		for (auto& cl : overlap_clusters) {
			move_cluster(cl, overlap_clusters, MBFF);
			cout << "move " << overlap_queue.front().get_clusterNum() << "\n";
			overlap_queue.pop();
		}
		cout << "finish!!!\n";
		overlap_clusters.clear();//overlap_clusters要初始化
		overlap_queue = get_overlap_clusters(MBFF, need_move_overlap_edges, overlap_clusters);
		if (shift_time > 100) {
			cout << "Queue elements: { ";
			while (!overlap_queue.empty()) {
				cout << overlap_queue.front().get_clusterNum() << " ";
				overlap_queue.pop();
			}
			cout << "}\n";
		}

	}
}

void legalize(vector<Cell>& MBFF, int bin_width, int bin_height, int die_x_min, int die_y_min, int die_x_max, int die_y_max) {
	binwidth = bin_width;
	binheight = bin_height;
	Die_max_x = die_x_max;
	Die_max_y = die_y_max;
	Die_min_x = die_x_min;
	Die_min_y = die_y_min;
	cout << "unit: " << bin_width << ", " << bin_height << "\n";
	for (auto& cell : MBFF) {
		int x = cell.getPos().access_Values().at(0);
		int y = cell.getPos().access_Values().at(1);
		cout << "x:" << x << ",\ty:" << y;
		to_the_site(x, y);
		Point new_position({ static_cast<float>(x), static_cast<float>(y) });

		cell.setPos(new_position);
		/*cell.set_xpos(x);  // 更新 x_pos、y_pos
		cell.set_ypos(y);*/
		cout << "\n";
	}
	cout << "finish to the site\n";

	vector<pair<Cell, Cell>> need_move_overlap_edges;
	vector<Cell> overlap_clusters;
	queue<Cell> overlap_queue;
	overlap_queue = get_overlap_clusters(MBFF, need_move_overlap_edges, overlap_clusters);

	shift_until_legal(overlap_clusters, overlap_queue, MBFF, need_move_overlap_edges);
}
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

using namespace std;

int BinWidth = 100;
int BinHeight = 100;
int BinMaxUtil = 75;
double sitewidth = 5.0;
double siteheight = 10.0;
int max_x = 50;
int max_y = 100;
int max_penalty = 10;

struct Event{
	double x;//x或y的座標
	int cluster_idx;//第幾個cluster
	bool is_start;//表示是開始(左邊)還是結束(右邊)

	bool operator<(const Event& other) const {
		if (x != other.x)
			return x < other.x;
		return is_start && !other.is_start;
	}
};

void to_the_site(double& x, double& y) {//到site上
	int new_x = x / sitewidth;
	int new_y = y / siteheight;
	x = sitewidth * new_x;
	y = siteheight * new_y;
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
void find_vertical_overlaps(vector<cluster>& clusters,unordered_map<int, unordered_set<int>>& overlap_graph) {
	vector<Event> events;
	for (int i = 0; i < clusters.size(); ++i) {
		events.push_back({ clusters[i].y_ordinate, i, true });
		events.push_back({ clusters[i].y_ordinate + clusters[i].C[0].height, i, false });
	}

	sort(events.begin(), events.end());

	set<int> active_clusters;
	
	for (const auto& event : events) {
		if (event.is_start) {
			for (int active_idx : active_clusters) {
				double active_end_y = clusters[active_idx].y_ordinate + clusters[active_idx].C[0].height;
				double event_end_y = clusters[event.cluster_idx].y_ordinate + clusters[event.cluster_idx].C[0].height;
				if (clusters[event.cluster_idx].y_ordinate < active_end_y && clusters[active_idx].y_ordinate < event_end_y) {
					overlap_graph[clusters[active_idx].cluster_name].insert(clusters[event.cluster_idx].cluster_name);
					overlap_graph[clusters[event.cluster_idx].cluster_name].insert(clusters[active_idx].cluster_name);
				}
			}
			active_clusters.insert(event.cluster_idx);
		}
		else {
			active_clusters.erase(event.cluster_idx);
		}
	}
}

void find_horizontal_overlaps(vector<cluster>& clusters, unordered_map<int, unordered_set<int>>& overlap_graph) {
	vector<Event> events;
	for (int i = 0; i < clusters.size(); ++i) {
		events.push_back({ clusters[i].x_ordinate, i, true });
		events.push_back({ clusters[i].x_ordinate + clusters[i].C[0].width, i, false });
	}

	sort(events.begin(), events.end());

	set<int> active_clusters;
	for (const auto& event : events) {
		if (event.is_start) {
			for (int active_idx : active_clusters) {
				double active_end_x = clusters[active_idx].x_ordinate + clusters[active_idx].C[0].width;
				double event_end_x = clusters[event.cluster_idx].x_ordinate + clusters[event.cluster_idx].C[0].width;
				if (clusters[event.cluster_idx].x_ordinate < active_end_x && clusters[active_idx].x_ordinate < event_end_x) {
					overlap_graph[clusters[active_idx].cluster_name].insert(clusters[event.cluster_idx].cluster_name);
					overlap_graph[clusters[event.cluster_idx].cluster_name].insert(clusters[active_idx].cluster_name);
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
void creat_overlap_edge(unordered_map<int, unordered_set<int>>& graph,int direction) {//建立vertical和horizontal overlap graph
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
void merge_horizontal_vertical_overlap(const set<pair<int, int>>& edges1, const set<pair<int, int>>& edges2, set<pair<int, int>>& overlap_edges, vector<pair<cluster, cluster>>& need_move_overlap_edges, set<int>& overlap_cluster, vector<cluster> clusters) {
	for (const auto& edge1 : edges1) {
		for (const auto& edge2 : edges2) {
			if ((edge1.first == edge2.first && edge1.second == edge2.second) || (edge1.first == edge2.second && edge1.second == edge2.first)) {
				//cout << "overlap edge: " << edge1.first << ", " << edge1.second << "\n";檢查overlap graph
				need_move_overlap_edges.push_back({ clusters[edge1.first], clusters[edge1.second] });  // 插入 pair<cluster, cluster>
				overlap_edges.insert(edge1);
				break;
			}
		}
	}
	for (const auto& edge1 : overlap_edges) {
		overlap_cluster.insert(edge1.first);
		overlap_cluster.insert(edge1.second);
	}
}

bool poriority_bigger(const cluster& cluster1, const cluster& cluster2) {
	//bit
	return cluster1.bit > cluster2.bit;
	//slack還沒有想法//
}
bool check_overlap(const cluster& cluster1, const cluster& cluster2) {
	double cluster1_x_start = cluster1.x_ordinate;
	double cluster1_x_end = cluster1.x_ordinate + cluster1.C[0].width;
	double cluster1_y_start = cluster1.y_ordinate;
	double cluster1_y_end = cluster1.y_ordinate + cluster1.C[0].height;

	double cluster2_x_start = cluster2.x_ordinate;
	double cluster2_x_end = cluster2.x_ordinate + cluster2.C[0].width;
	double cluster2_y_start = cluster2.y_ordinate;
	double cluster2_y_end = cluster2.y_ordinate + cluster2.C[0].height;

	// 看x, y方向有沒有overlap
	bool overlap_x = (cluster1_x_start < cluster2_x_end) && (cluster2_x_start < cluster1_x_end);
	bool overlap_y = (cluster1_y_start < cluster2_y_end) && (cluster2_y_start < cluster1_y_end);

	return overlap_x && overlap_y;
}

vector<cluster> find_nearest_overlap_cluster(cluster& current_cluster,  vector<cluster> overlap_clusters) {
	vector<cluster>nearest_overlap_cluster;
	double nearest_distance = 1000000000000000.0;//一個極大值
	for (const auto& cluster : overlap_clusters) {
		if (fabs(cluster.x_ordinate+cluster.C[0].width-current_cluster.x_ordinate-current_cluster.C[0].width)<nearest_distance&&current_cluster.cluster_name!=cluster.cluster_name) {
			if (check_overlap(cluster,current_cluster)) {//確認是否有overlap
				nearest_distance = fabs(cluster.x_ordinate + cluster.C[0].width - current_cluster.x_ordinate - current_cluster.C[0].width);
				nearest_overlap_cluster.clear(); // 清除之前的所有元素
				nearest_overlap_cluster.push_back(cluster); //添加最接近的overlap cluster
			}			
		}
	}
	return nearest_overlap_cluster;
}
bool samewidth( cluster& cluster1, cluster& cluster2) {
	double cluster1_x_start = cluster1.x_ordinate;
	double cluster1_x_end = cluster1.x_ordinate + cluster1.C[0].width;
	double cluster2_x_start = cluster2.x_ordinate;
	double cluster2_x_end = cluster2.x_ordinate + cluster2.C[0].width;
	return(cluster1_x_start == cluster2_x_start) && (cluster2_x_end == cluster1_x_end);
}
int completely_overlap_move = 0;
void move_cluster(cluster& current_cluster, const vector<cluster>& overlap_clusters, vector<cluster>& clusters) {	
	int unit_width = static_cast<int>(sitewidth);
	int unit_height = static_cast<int>(siteheight);
	vector<cluster> nearest_overlap = find_nearest_overlap_cluster(current_cluster, overlap_clusters);

	if (!nearest_overlap.empty()) {
		cluster& nearest_cluster = nearest_overlap[0]; // 提取第一個元素
		cout << "current cluster C"<<current_cluster.cluster_name<<" nearest cluster:" << "C"<<nearest_cluster.cluster_name<<"\n";
		if (!samewidth(nearest_cluster, current_cluster) && fabs(nearest_cluster.x_ordinate + nearest_cluster.C[0].width - current_cluster.x_ordinate - current_cluster.C[0].width) < fabs(nearest_cluster.y_ordinate + nearest_cluster.C[0].height - current_cluster.y_ordinate - current_cluster.C[0].height) && (nearest_cluster.p_left < max_penalty || nearest_cluster.p_right < max_penalty)) { // 确定水平或垂直移动
			cout << "horizontal move\n";
			if (nearest_cluster.x_ordinate + nearest_cluster.C[0].width < current_cluster.x_ordinate + current_cluster.C[0].width && nearest_cluster.p_right < max_penalty) { // 向右移动
				current_cluster.x_ordinate += unit_width;
				cout << "C" << current_cluster.cluster_name << " move right: " << current_cluster.x_ordinate << "\n";				
				current_cluster.p_right++;
				if (current_cluster.x_ordinate  > max_x || current_cluster.x_ordinate+current_cluster.C[0].width>max_x) {
					current_cluster.x_ordinate -= unit_width;
					cout << "C"<<current_cluster.cluster_name << " 到最右邊界\n";
				}
				//cout << "move right " << current_cluster.p_right << " times\n";確認移動幾次
				if (current_cluster.p_right >= max_penalty)
					cout << "C" << current_cluster.cluster_name << " can not move right!!!\n";//penalty太大不能再往右
			}
			else if (nearest_cluster.x_ordinate + nearest_cluster.C[0].width > current_cluster.x_ordinate + current_cluster.C[0].width && nearest_cluster.p_left < max_penalty) { // 向左移动
				current_cluster.x_ordinate -= unit_width;
				cout << "C" << current_cluster.cluster_name << " move left: " << current_cluster.x_ordinate << "\n";				
				current_cluster.p_left++;
				if (current_cluster.x_ordinate <0 || current_cluster.x_ordinate + current_cluster.C[0].width <0) {
					current_cluster.x_ordinate += unit_width;
					cout << "C" << current_cluster.cluster_name << " 到最左邊界:";
				}
				//cout << "move left " << current_cluster.p_left << " times\n";
				if (current_cluster.p_left >= max_penalty)
					cout << "C" << current_cluster.cluster_name << " can not move left!!!\n";//penalty太大不能再往左
			}
		}
		else { // 垂直移动		
			if (nearest_cluster.y_ordinate > current_cluster.y_ordinate) { // 向上移动
				cout << "C" << current_cluster.cluster_name << " move up\n";
				current_cluster.y_ordinate += unit_height;
				current_cluster.p_up++;
				if (current_cluster.y_ordinate >max_y || current_cluster.y_ordinate + current_cluster.C[0].height > max_y) {
					current_cluster.y_ordinate -= unit_height;
					cout << "C" << current_cluster.cluster_name << " 到最上邊界\n";
				}
				if (current_cluster.p_up >= max_penalty)
					cout << "C" << current_cluster.cluster_name << " can not move up!!!\n";//penalty太大不能再往上
			}
			else if (nearest_cluster.y_ordinate < current_cluster.y_ordinate) { // 向下移动
				cout << "C" << current_cluster.cluster_name << " move down\n";
				current_cluster.y_ordinate -= unit_height;
				current_cluster.p_down++;
				if (current_cluster.y_ordinate < 0 || current_cluster.y_ordinate + current_cluster.C[0].height < 0) {
					current_cluster.y_ordinate += unit_height;
					cout << "C" << current_cluster.cluster_name << " 到最下邊界\n";
				}
				if (current_cluster.p_down >= max_penalty)
					cout << "C" << current_cluster.cluster_name << " can not move down!!!\n";//penalty太大不能再往下
			}
			else if (nearest_cluster.y_ordinate == current_cluster.y_ordinate) { // 完全重疊
				cout << "C" << current_cluster.cluster_name << " completely overlap\n";
				current_cluster.x_ordinate += unit_width*completely_overlap_move;
				completely_overlap_move++;
			}
		}
	}
	else {
		cout << "No nearest overlap cluster found for C" << current_cluster.cluster_name << "\n";
	}
	for ( auto& cluster : clusters) {
		if (cluster.cluster_name == current_cluster.cluster_name) {
			cluster = current_cluster;
			break;
		}
	}
}

int shift_time = 0;//確認移動幾次而已
queue<cluster> get_overlap_clusters( vector<cluster>& clusters, vector<pair<cluster, cluster>>& need_move_overlap_edges, vector<cluster>& overlap_clusters)  {
	unordered_map<int, unordered_set<int>> overlap_horizontal_graph;
	unordered_map<int, unordered_set<int>> overlap_vertical_graph;
	//vector<cluster> overlap_clusters;
	queue<cluster>overlap_queue;
	find_horizontal_overlaps(clusters, overlap_horizontal_graph);	
	find_vertical_overlaps(clusters, overlap_vertical_graph);
	vertical_overlaps_edges.clear();
	horizontal_overlaps_edges.clear();
	creat_overlap_edge(overlap_vertical_graph, 1);//1:vettical,2:horizontal
	creat_overlap_edge(overlap_horizontal_graph, 2);
	set<pair<int, int>> overlap_edges;
	set<int> overlap_cluster;

	merge_horizontal_vertical_overlap(vertical_overlaps_edges, horizontal_overlaps_edges, overlap_edges, need_move_overlap_edges, overlap_cluster, clusters);
	if (overlap_cluster.size() < 1) {//完全legal!!!
		cout << "\n///////////congratulation///////////\n";
		cout << "\nvertical: \n";
		print_edges(vertical_overlaps_edges);
		cout << "horizontal:\n";
		print_edges(horizontal_overlaps_edges);
		cout << "Overlap clusters: {\n";
		for (const auto& cluster : overlap_cluster) {
			cout << cluster << " " << " start_x:" << clusters[cluster].x_ordinate << " ,start_y:" << clusters[cluster].y_ordinate << " " << "end_x:" << clusters[cluster].x_ordinate + clusters[cluster].C[0].width << " ,end_y:" << clusters[cluster].y_ordinate + clusters[cluster].C[0].height << "\n";
		}
		cout << "}\n";
		cout << "!!!legal!!!\n";
		return overlap_queue;
	}
	cout << "\nvertical: \n";
	print_edges(vertical_overlaps_edges);
	cout << "horizontal:\n";
	print_edges(horizontal_overlaps_edges);
	cout << "Overlap clusters: {\n";
	for (const auto& cluster : overlap_cluster) {
		cout << cluster << " "<<" start_x:"<<clusters[cluster].x_ordinate<<" ,start_y:"<<clusters[cluster].y_ordinate<<" "<<"end_x:"<< clusters[cluster].x_ordinate +clusters[cluster].C[0].width<<" ,end_y:"<< clusters[cluster].y_ordinate + clusters[cluster].C[0].height<<"\n";
	}
	cout << "}\n";
	for (int cluster_idx : overlap_cluster) {
		overlap_clusters.push_back(clusters[cluster_idx]);
	}
	sort(overlap_clusters.begin(), overlap_clusters.end(), poriority_bigger);
	for (auto& cl : overlap_clusters) {//把cluster轉換成queue
		overlap_queue.push(cl);
	}
	cout << "\nafter " <<shift_time<< " time shift:\n";
	shift_time++;
	cout << "overlap clusters: ";
	for (auto& cluster : overlap_clusters) {
		cout << "C" << cluster.cluster_name << " ";
	}
	cout<<"\n\n";
	return overlap_queue;
}
 
void shift_until_legal(vector<cluster> overlap_clusters,queue<cluster>& overlap_queue,vector<cluster>& clusters, vector<pair<cluster, cluster>>& need_move_overlap_edges) {
	while (!overlap_queue.empty()) {		
		for (auto& cl : overlap_clusters) {
			move_cluster(cl, overlap_clusters, clusters);
			cout <<"move " << overlap_queue.front().cluster_name << "\n";
			overlap_queue.pop();
		}
		cout << "finish!!!\n";
		overlap_clusters.clear();//overlap_clusters要初始化
		overlap_queue = get_overlap_clusters(clusters, need_move_overlap_edges,overlap_clusters);
		if (shift_time > 30) {
			cout << "Queue elements: { ";
			while (!overlap_queue.empty()) {
				cout << overlap_queue.front().cluster_name << " ";
				overlap_queue.pop();
			}
			cout << "}\n";
		}
		
	}
}

void legalize (vector<cluster>& clusters){
	
	for (int i = 0; i < clusters.size(); i++) {
		cout <<"C"<<i << ", x:" << clusters[i].x_ordinate << ",\ty:" << clusters[i].y_ordinate<<",\tbit:"<<clusters[i].bit;
		to_the_site(clusters[i].x_ordinate, clusters[i].y_ordinate);
		cout << "\n";
	}

	vector<pair<cluster, cluster>> need_move_overlap_edges;
	vector<cluster> overlap_clusters;
	queue<cluster> overlap_queue;
	overlap_queue= get_overlap_clusters(clusters, need_move_overlap_edges, overlap_clusters);
		
	shift_until_legal(overlap_clusters,overlap_queue, clusters, need_move_overlap_edges);
}


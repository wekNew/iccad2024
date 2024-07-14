#include "partition.h"


void clusterToMBFF(vector<Cell*>& best_st_table,vector<Cell*>& cells, Point& cluster_pos,vector<combination>& combi_table, vector<Cell>& MBFF, int st_size, int rm_size) {
	cout << "start to clusterToMBFF" << endl;
	if (rm_size != -1) {
		
		int centroid1 = findFarthestPtoP(cells, cluster_pos);
		int centroid2 = findFarthestPtoP(cells, cells[centroid1]->get_pos_address());
		
		// Initial centroids
		vector<Point> centroids;
		centroids.push_back(cells[centroid1]->getPos());
		centroids.push_back(cells[centroid2]->getPos());

		int maxSize1 = rm_size;
		int maxSize2 = st_size;

		int k = centroids.size();
		vector<int> clusterAssignment(cells.size());

		// Assign initial cluster sizes based on initial centroids
		vector<int> clusterSize(k, 0);
		for (int i = 0; i < cells.size(); ++i) {
			//double dist1 = distance(cells[i]->getPos(), centroids[0]);
			//double dist2 = distance(cells[i]->getPos(), centroids[1]);

			double dist1 = cells[i]->getPos().euclideanDistance(centroids[0]);
			double dist2 = cells[i]->getPos().euclideanDistance(centroids[1]);

			if (dist1 < dist2) {
				clusterAssignment[i] = 0;
				clusterSize[0]++;
			}
			else {
				clusterAssignment[i] = 1;
				clusterSize[1]++;
			}
		}

		// Adjust cluster sizes if needed based on initial assignment
		int largerCluster, smallerCluster;
		if (clusterSize[0] >= clusterSize[1]) {
			largerCluster = 0;
			smallerCluster = 1;
		}
		else {
			largerCluster = 1;
			smallerCluster = 0;
		}

		int st_cluster = smallerCluster;
		int rm_cluster = largerCluster;

		// Ensure maxSize1 is the larger one
		if (maxSize1 < maxSize2) {
			swap(maxSize1, maxSize2);
			swap(st_cluster, rm_cluster);
		}

		// Adjust initial cluster sizes
		if (clusterSize[smallerCluster] > maxSize2) {
			while (clusterSize[smallerCluster] > maxSize2) {
				// Find farthest point from larger cluster's centroid
				int farthestIndex = findFarthestPointFromCentroid(cells, clusterAssignment, smallerCluster, centroids[smallerCluster]);

				if (farthestIndex != -1) {
					// Move the farthest point to the smaller cluster
					clusterAssignment[farthestIndex] = largerCluster;
					clusterSize[smallerCluster]--;
					clusterSize[largerCluster]++;
				}
			}
		}
		else if (clusterSize[largerCluster] > maxSize1) {
			while (clusterSize[largerCluster] > maxSize1) {
				// Find farthest point from larger cluster's centroid
				int farthestIndex = findFarthestPointFromCentroid(cells, clusterAssignment, largerCluster, centroids[largerCluster]);

				if (farthestIndex != -1) {
					// Move the farthest point to the smaller cluster
					clusterAssignment[farthestIndex] = smallerCluster;
					clusterSize[largerCluster]--;
					clusterSize[smallerCluster]++;
				}
			}
		}

		// Calculate centroids based on current cluster assignments
		Point tmp = {-1,-1};
		vector<Point> newCentroids(k, tmp);
		clusterSize.assign(k, 0);

		// Update centroids
		for (int i = 0; i < cells.size(); ++i) {
			int cluster = clusterAssignment[i];
			newCentroids[cluster].access_Values().at(0) += cells[i]->getPos().access_Values().at(0);
			newCentroids[cluster].access_Values().at(1) += cells[i]->getPos().access_Values().at(1);
			clusterSize[cluster]++;
		}
		for (int j = 0; j < k; ++j) {
			if (clusterSize[j] > 0) {
				newCentroids[j].access_Values().at(0) /= clusterSize[j];
				newCentroids[j].access_Values().at(1) /= clusterSize[j];
			}
		}


		//debug
		{
			// Output final cluster assignments for each point
			cout << "Final cluster assignments:" << endl;
			for (int i = 0; i < cells.size(); ++i) {
				cout << "Point " << i << " -> Cluster " << clusterAssignment[i] << endl;
			}
			// Output final centroids
			cout << "Final centroids:" << endl;
			for (int i = 0; i < centroids.size(); ++i) {
				cout << "Centroid " << i << ": (" << centroids[i].access_Values().at(0) << ", " << centroids[i].access_Values().at(1) << ")" << endl;
			}
		}

		//build mbff
		Cell mbff_tmp(-1, "notyet", -1, -1, -1);
		mbff_tmp.setPos(newCentroids[st_cluster]);
		vector<Cell*> rm_cells;
		for (int i = 0; i < clusterAssignment.size(); ++i) {
			if (clusterAssignment[i] == st_cluster) {
				mbff_tmp.get_children().push_back(cells[i]);
			}
			else {
				rm_cells.push_back(cells[i]);
			}
		}
		mbff_tmp.set_bit(mbff_tmp.get_children().size());
		MBFF.push_back(mbff_tmp);
		clusterToMBFF(rm_cells, newCentroids[rm_cluster],combi_table, MBFF, combi_table[rm_cells.size()].combi_1, combi_table[rm_cells.size()].combi_2);
	}
	else {
		//build mbff
		Cell mbff_tmp(cells.size(),"notyet",-1,-1,-1);
		//mbff_tmp.bit = cells.size();
		mbff_tmp.setPos(cluster_pos);
		mbff_tmp.set_children(cells);
		MBFF.push_back(mbff_tmp);
	}
}



int findFarthestPtoP(vector<Cell*>& cells, Point& p) {
	//cout << "start to findFarthestP2P" << endl;
	double maxDist = -1;
	int farthestIndex = -1;
	//cout << "p_pos = " << p.access_Values().at(0) << "," << p.access_Values().at(1) << endl;
	for (int i = 0; i < cells.size(); i++) {
		cout << "i : " << i << endl;
		cout << cells[i]->getPos().access_Values().at(0) << "," << cells[i]->getPos().access_Values().at(1) << endl;
		//double dist = distance(cells[i]->get_pos_address(), p);
		double dist = cells[i]->getPos().euclideanDistance(p);
		
		if (dist > maxDist) {
			maxDist = dist;
			farthestIndex = i;
		}
	}
	return farthestIndex;
}

double distance(Point& a, Point& b) {
	return sqrt(pow(a.access_Values().at(0) - b.access_Values().at(0), 2) + pow(a.access_Values().at(1) - b.access_Values().at(1), 2));
}

int findFarthestPointFromCentroid(vector<Cell*>& cells, vector<int>& clusterAssignment, int cluster, Point& centroid) {
	double maxDist = -1;
	int farthestIndex = -1;

	for (int i = 0; i < cells.size(); ++i) {
		
		if (clusterAssignment[i] == cluster) {
			//double dist = distance(cells[i]->getPos(), centroid);
			double dist = cells[i]->getPos().euclideanDistance(centroid);
			if (dist > maxDist) {
				maxDist = dist;
				farthestIndex = i;
			}
		}
	}

	return farthestIndex;
}

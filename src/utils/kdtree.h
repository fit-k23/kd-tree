#ifndef KD_TREE_KDTREE_H
#define KD_TREE_KDTREE_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include <fstream>
#include <string>

using namespace std;

struct Data{
	std::string city;
	double latitude, longitude;
};

struct KDTree{
	Data data;
	KDTree *left, *right;
};

bool insertData(KDTree *&root, Data &data, int depth = 0) {
	if (root == nullptr) {
		root = new KDTree{data, nullptr, nullptr};
		return true;
	}
	if (depth % 2 == 0) {
		if (data.latitude == root->data.latitude) return false;
		return insertData((data.latitude < root->data.latitude) ? root->left : root->right, data, depth + 1);
	}
	if (data.longitude == root->data.longitude) return false;
	return insertData((data.longitude < root->data.longitude) ? root->left : root->right, data, depth + 1);
}

void nearestNeighborSearch(double latitude, double longitude);

void rangeQuery(double leftLat, double leftLong, double rightLat, double rightLong);

KDTree* readCSVFile(const string& filePath) {
	ifstream file(filePath.c_str());
	if (!file.is_open()) {
		return nullptr;
	}
	KDTree *tree{nullptr};
	string tmp;
	getline(file, tmp); // skip header
	while (getline(file, tmp)) {
		Data data;
		size_t p1 = tmp.find(',');
		size_t p2 = tmp.find(',', p1 + 1);
		data.city = tmp.substr(0, p1);
		if (p2 <= p1) continue;
		data.latitude = stod(tmp.substr(p1 + 1, p2 - p1 - 1));
		p1 = tmp.find(' ', p2 + 1);
		if (p1 <= p2) continue;
		data.longitude = stod(tmp.substr(p2 + 1, p1 - p2 - 1));
		insertData(tree, data);
	}
	file.close();
	return tree;
}

void printKDTree(KDTree *root = nullptr, string prefix = "", bool isLeft = false) {
	if (root != nullptr) {
		cout << prefix;
		cout << (isLeft ? "├──" : "└──");
//		cout << (isLeft ? "─" : "|");

		// print the value of the node
		cout << " " << root->data.city << " - (" << root->data.latitude << "; " << root->data.longitude <<")\n";

		// enter the next tree level - left and right branch
		printKDTree(root->left, prefix + (isLeft ? "│   " : "    "), true);
		printKDTree(root->right, prefix + (isLeft ? "│   " : "    "), false);
	}
}

#pragma clang diagnostic pop
#endif //KD_TREE_KDTREE_H
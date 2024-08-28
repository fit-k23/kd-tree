#ifndef KD_TREE_KDTREE_H
#define KD_TREE_KDTREE_H

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

struct Data{ // NOLINT(*-pro-type-member-init)
	std::string city;
	double latitude, longitude;
};

struct KDTree{ // NOLINT(*-pro-type-member-init)
	Data data;
	KDTree *left, *right;
};

double getDist(Data, Data);
void nearestNeighborSearch(KDTree *, const Data &, int, bool, double &, Data &);
bool isInRange(const Data &, double, double, double, double);
void rangeQuery(KDTree *, double, double, double, double, int);
vector<Data> readCSVFile(const string &filePath);

void printKDTree(KDTree *root = nullptr, const string &prefix = "", bool isLeft = false) {
	if (root != nullptr) {
		cout << prefix;
		cout << (isLeft ? "├──" : "└──");

		// print the value of the node
		cout.precision(4);
		cout << root->data.city << " - (" << fixed << root->data.latitude << "; " << root->data.longitude << ")\n";

		// enter the next tree level - left and right branch
		printKDTree(root->left, prefix + (isLeft ? "│   " : "    "), true);
		printKDTree(root->right, prefix + (isLeft ? "│   " : "    "), false);
	}
}

void deleteTree(KDTree *&root) {
	if (root == nullptr) return;
	deleteTree(root->left);
	deleteTree(root->right);
	delete root;
	root = nullptr;
}

struct DataCompare{
	int axis;

	DataCompare(int axis) : axis(axis) {} // NOLINT(*-explicit-constructor)
	bool operator()(const Data &d1, const Data &d2) const {
		return (axis == 0 ? d1.latitude < d2.latitude : d1.longitude < d2.longitude);
	}
};

KDTree *buildKDTree(vector<Data> &dataset, long long l = 0, long long r = 1, int depth = 0) {
	if (r >= dataset.size() || r < l) {
		return nullptr;
	}
	sort(dataset.begin() + l, dataset.begin() + r + 1, DataCompare(depth % 2));
	long long m = (l + r) / 2;

	return new KDTree{
		dataset[m],
		buildKDTree(dataset, l, m - 1, depth + 1),
		buildKDTree(dataset, m + 1, r, depth + 1)
	};
}

bool insertData(KDTree *&root, Data &data, int depth = 0) {
	if (root == nullptr) {
		root = new KDTree{data, nullptr, nullptr};
		return true;
	}
	if (depth % 2 == 0) {
		return insertData((data.latitude < root->data.latitude) ? root->left : root->right, data, depth + 1);
	}
	return insertData((data.longitude < root->data.longitude) ? root->left : root->right, data, depth + 1);
}

void NLR_Vectorify(KDTree *root, vector<Data> &dataset) {
	if (root == nullptr) return;
	dataset.push_back(root->data);
	NLR_Vectorify(root->left, dataset);
	NLR_Vectorify(root->right, dataset);
}

// Insert then rebuild
void insertDataBalance(KDTree *&root, Data data) {
	vector<Data> dataset;
	NLR_Vectorify(root, dataset);
//	cout << "Insert (" << data.city << ", " << data.latitude << ", " << data.longitude << ")\n";
	dataset.push_back(data);
	deleteTree(root);
	root = buildKDTree(dataset, 0, (long long) dataset.size() - 1);
}

void insertBalanceFromCSV(KDTree *& root, const string &filePath) {
	vector<Data> dataset = readCSVFile(filePath);
	NLR_Vectorify(root, dataset);
	deleteTree(root);
	root = buildKDTree(dataset, 0, (long long) dataset.size() - 1);
}

double getDist(Data x, Data y) {
	// compute latitude and longitude distance
	double distLat = (y.latitude - x.latitude) * M_PI / 180.0;
	double distLong = (y.longitude - x.longitude) * M_PI / 180.0;

	// convert to radians
	x.latitude = (x.latitude * M_PI) / 180.0;
	y.latitude = (y.latitude * M_PI) / 180.0;

	// formula
	double a = pow(sin(distLat / 2), 2) + pow(sin(distLong / 2), 2) * cos(x.latitude) * cos(y.latitude);
	double rad = 6371;
	double c = 2 * asin(sqrt(a));
	return rad * c;
}

void nearestNeighborSearch(KDTree *root, const Data &targ, int depth, bool noCandidate, double &bestDist, Data &bestData) {
	if (root == nullptr) return;

	double dist = getDist(root->data, targ);
	if (noCandidate || dist < bestDist) {
		bestDist = dist;
		bestData = root->data;
		noCandidate = false;
	}
	if (bestDist == 0) return;

	double distDim = (depth % 2 == 0 ? root->data.latitude - targ.latitude : root->data.longitude - targ.longitude);
	(depth += 1) %= 2;
	nearestNeighborSearch(distDim > 0 ? root->left : root->right, targ, depth, noCandidate, bestDist, bestData);
	if ((long double) distDim * (long double) distDim >= bestDist) return;
	nearestNeighborSearch(distDim > 0 ? root->right : root->left, targ, depth, noCandidate, bestDist, bestData);
}

bool isInRange(const Data &city, double leftLat, double leftLong, double rightLat, double rightLong) {
	return city.latitude >= leftLat && city.latitude <= rightLat && city.longitude >= leftLong && city.longitude <= rightLong;
}

void rangeQuery(KDTree *root, double leftLat, double leftLong, double rightLat, double rightLong, int depth) {
	if (root == nullptr) return;
	if (isInRange(root->data, leftLat, leftLong, rightLat, rightLong)) {
		cout << "City (" << root->data.city << ", " << root->data.latitude << ", " << root->data.longitude << ") is in range\n";
	}
	if ((depth % 2 == 0 && root->data.latitude > leftLat) || (depth % 2 == 1 && root->data.longitude > leftLong)) {
		rangeQuery(root->left, leftLat, leftLong, rightLat, rightLong, depth + 1);
	}
	if ((depth % 2 == 0 && root->data.latitude < rightLat) || (depth % 2 == 1 && root->data.longitude < rightLong)) {
		rangeQuery(root->right, leftLat, leftLong, rightLat, rightLong, depth + 1);
	}
}

vector<Data> readCSVFile(const string &filePath) {
	ifstream file(filePath.c_str());
	vector<Data> dataset;
	if (!file.is_open()) {
		cout << "Failed to read files\n";
		return dataset;
	}
	string tmp;
	getline(file, tmp); // skip header
	while (getline(file, tmp)) {
		Data data;
		size_t p1 = tmp.find(',');
		size_t p2 = tmp.find(',', p1 + 1);
		data.city = tmp.substr(0, p1);
		if (p2 <= p1) continue;
		data.latitude = stod(tmp.substr(p1 + 1, p2 - p1 - 1));
		p1 = tmp.find(',', p2 + 1);
		if (p1 <= p2) continue;
		data.longitude = stod(tmp.substr(p2 + 1, p1));
		dataset.push_back(data);
	}
	file.close();
	return dataset;
}

bool writeCSVFromTree(KDTree *root, const string &filePath) {
	ofstream file(filePath.c_str());
	if (!file.is_open()) {
		return false;
	}
	vector<Data> dataset;
	NLR_Vectorify(root, dataset);
	file << "city,lat,lng\n";
	for (auto & data : dataset) {
		file << data.city << "," << data.latitude << "," << data.longitude << "\n";
	}
	return true;
}

KDTree* readCSVFileIntoTree(const string &filePath) {
	vector<Data> dataset = readCSVFile(filePath);
	if (dataset.empty()) {
		return nullptr;
	}
	return buildKDTree(dataset, 0, dataset.size() - 1);
}

#include "json.hpp"
// Convert KD Tree to json file

nlohmann::json tree_to_json(KDTree *root) {
	if (root == nullptr) {
		return nullptr;
	}

	nlohmann::json j;
	j["data"] = nlohmann::json{
		{"city",      root->data.city},
		{"latitude",  root->data.latitude},
		{"longitude", root->data.longitude}
	};
	j["left"] = tree_to_json(root->left);
	j["right"] = tree_to_json(root->right);

	return j;
}

bool saveKDTree(const string &filePath, KDTree *root) {
	ofstream file(filePath.c_str());
	if (!file.is_open()) {
		return false;
	}
	nlohmann::json j = tree_to_json(root);
	if (j == nullptr) {
		return false;
	}
	file << j.dump(1, '\t');
	file.close();
	return true;
}

Data data_from_json(const nlohmann::json &j) {
	return {
		j.at("city").get<std::string>(),
		j.at("latitude").get<double>(),
		j.at("longitude").get<double>()
	};
}

KDTree *tree_from_json(const nlohmann::json &j) {
	if (j.is_null()) {
		return nullptr;
	}

	auto *root = new KDTree;

	if (j.contains("data")) {
		root->data = data_from_json(j.at("data"));
	}

	if (j.contains("left")) {
		root->left = tree_from_json(j.at("left"));
	}
	if (j.contains("right")) {
		root->right = tree_from_json(j.at("right"));
	}
	return root;
}

KDTree *loadKDTree(const string &filePath) {
	ifstream file(filePath.c_str());
	if (!file.is_open()) {
		return nullptr;
	}
	KDTree *root = tree_from_json(nlohmann::json::parse(file));
	file.close();
	return root;
}

#pragma clang diagnostic pop
#endif //KD_TREE_KDTREE_H

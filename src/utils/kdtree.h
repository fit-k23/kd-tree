#ifndef KD_TREE_KDTREE_H
#define KD_TREE_KDTREE_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct Data{ // NOLINT(*-pro-type-member-init)
	std::string city;
	double latitude, longitude;
};

struct KDTree{ // NOLINT(*-pro-type-member-init)
	Data data;
	KDTree *left, *right;
};

void deleteTree(KDTree *root) {
	if (root == nullptr) return;
	deleteTree(root->left);
	deleteTree(root->right);
	delete root;
}

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
		p1 = tmp.find(',', p2 + 1);
		if (p1 <= p2) continue;
		data.longitude = stod(tmp.substr(p2 + 1, p1));
		insertData(tree, data);
	}
	file.close();
	return tree;
}

void printKDTree(KDTree *root = nullptr, const string& prefix = "", bool isLeft = false) {
	if (root != nullptr) {
		cout << prefix;
		cout << (isLeft ? "├──" : "└──");

		// print the value of the node
		cout.precision(4);
		cout << root->data.city << " - (" << fixed << root->data.latitude << "; " << root->data.longitude <<")\n";

		// enter the next tree level - left and right branch
		printKDTree(root->left, prefix + (isLeft ? "│   " : "    "), true);
		printKDTree(root->right, prefix + (isLeft ? "│   " : "    "), false);
	}
}

#include "json.hpp"

// Convert KD Tree to json file

nlohmann::json tree_to_json(KDTree *root) {
	if (root == nullptr) {
		return nullptr;
	}

	nlohmann::json j;
	j["data"] = nlohmann::json{
		{"city", root->data.city},
		{"latitude", root->data.latitude},
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

KDTree* tree_from_json(const nlohmann::json& j) {
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

KDTree* loadKDTree(const string &filePath) {
	ifstream file(filePath.c_str());
	KDTree* root = tree_from_json(nlohmann::json::parse(file));
	file.close();
	return root;
}

#pragma clang diagnostic pop
#endif //KD_TREE_KDTREE_H
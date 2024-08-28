#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include "utils/kdtree.h"

using namespace std;

KDTree *tree = nullptr; // main KD-Tree

void progressLoading();

void printOption();

void handleUserInput(bool &);

// COMMAND LINE FUNCTION

void progressLoading() { // just for user interface
	for (int i = 0; i < 3; ++i) {
		cout << '.';
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	cout << '\n';
}

void printOption() {
	cout << "-----------------------------------------------------\n";
	cout << "Here are your options:\n";
	cout << " 1) Load the list of cities from a CSV file database.\n";
	cout << " 2) Insert a new city into KD-Tree.\n";
	cout << " 3) Insert multiple cities via specified CSV path.\n";
	cout << " 4) Nearest-neighbor search based on giving latitude and longitude.\n";
	cout << " 5) Query cities within a specified rectangular region\n";
	cout << " 6) ========= Quit =========\n";
	cout << "       ADVANCED FEATURES    \n";
	cout << " 7) Print current tree (if exist)\n";
	cout << " 8) Save tree to JSON file\n";
	cout << " 9) Load tree from JSON file\n";
	cout << "10) Save tree to CSV file\n";
	cout << "Your option: ";
}

void handleUserInput(bool &userLoop) {
	int opt; // user option
	cin >> opt;
	if (opt == 1) {
		progressLoading();
		cout << "Complete loading dataset\n";
		if (tree == nullptr) {
			tree = readCSVFileIntoTree("./worldcities.csv");
			if (tree == nullptr) {
				cout << "Cannot find file worldcities.csv in working directory.\n";
				return;
			}
		}
	} else if (opt == 2) {
		string city;
		double latitude, longitude;
		cout << "City name: ";
		cin.ignore();
		getline(cin, city);
		cout << "Latitude: ";
		cin >> latitude;
		cout << "Longitude: ";
		cin >> longitude;
		cout << "Insert (" << city << ", " << latitude << ", " << longitude << ") into KD-Tree\n";
		insertDataBalance(tree, {city, latitude, longitude});
	} else if (opt == 3) {
		string csvPath;
		cout << "Enter csv file path: ";

		cin.ignore();
		getline(cin, csvPath);

		ifstream fin(csvPath.c_str());
		if (!fin.is_open()) {
			cout << "Cannot open file\n";
		} else {
			string filename, ext;
			for (int i = (int) csvPath.size() - 1; i >= 0; --i) {
				if (csvPath[i] == '/') break;
				filename += csvPath[i];
			}
			for (int i = (int) csvPath.size() - 1; i >= 0; --i) {
				if (csvPath[i] == '.') break;
				ext += csvPath[i];
			}
			reverse(filename.begin(), filename.end());
			reverse(ext.begin(), ext.end());

			if (ext != "csv") {
				cout << "Not a csv file\n";
			} else {
				progressLoading();
				cout << "Complete loading csv file\n";
				insertBalanceFromCSV(tree, csvPath);
			}
		}
		if (fin.is_open()) fin.close();
	} else if (opt == 4) {
		if (tree == nullptr) {
			cout << "Tree is empty\n";
		} else {
			double latitude, longitude;
			cout << "Latitude: ";
			cin >> latitude;
			cout << "Longitude: ";
			cin >> longitude;
			double bestDist = 0;
			Data bestCity, targ = {"", latitude, longitude};

			nearestNeighborSearch(tree, targ, 0, true, bestDist, bestCity);
			cout << "Closet city to your location is (" << bestCity.city << ", " << bestCity.latitude << ", " << bestCity.longitude << ") with distance " << bestDist << '\n';
		}
	} else if (opt == 5) {
		if (tree == nullptr) {
			cout << "Tree is empty\n";
		} else {
			double bottomLeftLat, bottomLeftLong;
			double topRightLat, topRightLong;
			cout << "Bottom-left latitude: ";
			cin >> bottomLeftLat;
			cout << "Bottom-left longitude: ";
			cin >> bottomLeftLong;
			cout << "Top-right latitude: ";
			cin >> topRightLat;
			cout << "Top-right longitude: ";
			cin >> topRightLong;
			cout << "Output csv [Enter to skip]: ";
			cin.ignore();
			string outputFile;
			getline(cin, outputFile);

			vector<Data> queries = {};
			rangeQuery(tree, queries, bottomLeftLat, bottomLeftLong, topRightLat, topRightLong, 0);
			for (auto &query : queries) {
				cout << "City (" << query.city << ", " << query.latitude << ", " << query.longitude << ") is in range\n";
			}
			if (!outputFile.empty()) {
				cout << "Saved output to file (" << outputFile << ")\n";
				writeCSVFile(queries, outputFile);
			}
		}
	} else if (opt == 6) {
		cout << "Quitting...\n";
		userLoop = false;
	} else if (opt == 7) {
		cout << "Tree visualize: " << (tree == nullptr ? "empty" : "") << "\n";
		if (tree != nullptr) {
			printKDTree(tree);
		}
	} else if (opt == 8) {
		cout << "Output JSON file: ";
		cin.ignore();
		string filePath;
		getline(cin, filePath);
		if (!saveKDTree(filePath, tree)) {
			cout << "Failed to save tree to file " << filePath << "\n";
		} else {
			cout << "Succeed to save tree to file " << filePath << "\n";
		}
	} else if (opt == 9) {
		cout << "Input JSON file: ";
		cin.ignore();
		string filePath;
		getline(cin, filePath);
		KDTree *nTree = loadKDTree(filePath);
		if (nTree == nullptr) {
			cout << "Failed to load tree from file " << filePath << "\n";
		} else {
			deleteTree(tree);
			tree = nTree;
			cout << "Succeed to load tree from file " << filePath << "\n";
		}
	} else if (opt == 10) {
		if (tree == nullptr) {
			cout << "Tree is empty\n";
		} else {
			cout << "Output CSV file: ";
			cin.ignore();
			string filePath;
			getline(cin, filePath);
			writeCSVFromTree(tree, filePath);
		}
	} else {
		cout << "Invalid option\n";
	}
}

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
	#ifdef _WIN32
	SetConsoleOutputCP(65001);
	#endif

	bool userLoop = true;
	while (userLoop) {
		printOption();
		handleUserInput(userLoop);
	}

	deleteTree(tree); // granted to free out the tree
	return 0;
}
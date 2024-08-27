#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include "windows.h"
using namespace std;

double Simplemap_loaded = false;

struct Node {
    string city;
    double latitude, longitude;
};

struct KDTree {

};

string readUserString();
void progressLoading();
void printOption();
void handleUserInput();
void loadSimplemap();
void loadCSVFile(string);
void insertNode(Node);
void nearestNeighborSearch(double, double);
void rangeQuery(double, double, double, double);

// KD TREE FUNCTION
void insertNode(Node node) {

}

void nearestNeighborSearch(double latitude, double longitude) {

}

void rangeQuery(double leftLat, double leftLong, double rightLat, double rightLong) {

}

// COMMAND LINE FUNCTION
string readUserString() {
    string ret = "";
    string tmp;
    bool first = true;
    while (cin >> tmp) {
        if (first == false) ret += ' ';
        ret += tmp;
        first = false;
    }
    return ret;
}

void progressLoading() {
    for (int i = 0; i < 3; ++i) {
        cout << '.';
        Sleep(100);
    }
    cout << '\n';
}

void printOption() {
    cout << "-----------------------------------------------------\n";
    cout << "Here are your options:\n";
    cout << "1) Load the list of cities from a CSV file of Simplemaps database.\n";
    cout << "2) Insert a new city into KD-Tree.\n";
    cout << "3) Insert multiple cities via specified CSV path.\n";
    cout << "4) Nearest-neighbor search based on giving latitude and longitude.\n";
    cout << "5) Query cities within a specified rectangular region\n";
    cout << "Your option: ";
}

void handleUserInput() {
    int opt; // user option
    cin >> opt;
    if (opt == 1) {
        progressLoading();
        cout << "Complete loading Simplemaps database\n";
        if (Simplemap_loaded == false) {
            loadSimplemap();
            Simplemap_loaded = true;
        }
    }
    else if (opt == 2) {
        string city;
        double latitude, longitude;
        cout << "City name: ";
        city = readUserString();
        cout << "Latitude: ";
        cin >> latitude;
        cout << "Longitude: ";
        cin >> longitude;
        cout << "Insert (" << city << ", " << latitude << ", " << longitude << ") into KD-Tree\n";
        insertNode({city, latitude, longitude});
    }
    else if (opt == 3) {
        string csvPath;
        cout << "Enter csv file path: ";
        
        csvPath = readUserString();

        ifstream fin(csvPath.c_str());
        if (!fin.is_open()) {
            cout << "Cannot open file\n";
        }
        else {
            string filename, ext;
            for (int i = (int)csvPath.size() - 1; i >= 0; --i) {
                if (csvPath[i] == '/') break;
                filename += csvPath[i];
            }
            for (int i = (int)csvPath.size() - 1; i >= 0; --i) {
                if (csvPath[i] == '.') break;
                ext += csvPath[i];
            }
            reverse(filename.begin(), filename.end());
            reverse(ext.begin(), ext.end());

            if (ext != "csv") {
                cout << "Not a csv file\n";
            }
            else {
                progressLoading();
                cout << "Complete loading csv file\n";
                if (filename == "worldcities.csv") {
                    if (!Simplemap_loaded) {
                        loadSimplemap();
                        Simplemap_loaded = true;
                    }
                }
                else loadCSVFile(csvPath);
            }
        }
        if (fin.is_open()) fin.close();
    }
    else if (opt == 4) {
        double latitude, longitude;
        cout << "Latitude: ";
        cin >> latitude;
        cout << "Longitude: "; 
        cin >> longitude;

        nearestNeighborSearch(latitude, longitude);
    }
    else if (opt == 5) {
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
        rangeQuery(bottomLeftLat, bottomLeftLong, topRightLat, topRightLong);
    }
    else {
        cout << "Invalid option\n";
    }
}

void loadSimplemap() {

}

void loadCSVFile(string path) {
     
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);



    return 0;
}
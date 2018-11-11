#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

void setVector(vector<int> &vec) {
	for (int n = 1; n < 50; n++) {
		int num = n*(n + 1) / 2;
		vec.push_back(num);
	}
}

int Eureka(vector<int> &vec, int input_num) {
	for (int i = 0; i < vec.size(); i++) {
		for (int j = i; j < vec.size(); j++) {
			for (int k = j; k < vec.size(); k++) {
				if (vec[i] + vec[j] + vec[k] == input_num) {
					return 1;
				}
			}
		}
	}
	return 0;
}

int main() {
	ifstream fin;
	fin.open("input.txt");
	int numTestCase;
	fin >> numTestCase;

	vector<int> triangle_num;
	setVector(triangle_num);
	
	for (int i = 0; i < numTestCase; i++) {
		int input_num;
		fin >> input_num;

		cout << Eureka(triangle_num, input_num) << endl;

	}
	triangle_num.clear();
	fin.close();
	return 0;
}
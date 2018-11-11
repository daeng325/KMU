#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

bool compare(int a, int b) {
	return a > b;
}

int get_size_of_room(char **arr, int n, int m, char _old, char _new) {
	if (arr[n][m] != _old) {
		return 0;
	}
	else {
		arr[n][m] = _new;
		int result = 1;
		result += get_size_of_room(arr, n - 1, m, _old, _new);
		result += get_size_of_room(arr, n + 1, m, _old, _new);
		result += get_size_of_room(arr, n , m-1 , _old, _new);
		result += get_size_of_room(arr, n, m + 1, _old, _new);
		return result;
	}
}

void set_arr(char **arr,int n, int m, ifstream &fin) {
	for (int j = 0; j < n; j++) {
		for (int k = 0; k < m; k++) {
			char str;
			fin >> str;
			arr[j][k] = str;
		}
	}
}

int main() {
	ifstream fin;
	fin.open("input.txt");

	int numTestCase;
	fin >> numTestCase;
	for (int i = 0; i < numTestCase; i++) {
		int m, n;
		fin >> m >> n;
		char **arr = new char*[n];

		for (int j = 0; j < n; j++)
		{
			arr[j] = new char[m];
		}

		set_arr(arr, n, m, fin);
		//char num = '1';
		vector<int> vec;
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < m; k++) {
				int result = get_size_of_room(arr, j, k, '.', '+');
				if (result != 0) {
					vec.push_back(result);
				}
			}
		}

		sort(vec.begin(), vec.end(),compare);
		
		cout << vec.size() << endl;
		for (int j = 0; j < vec.size(); j++) {
			cout << vec[j] << " ";
		}
		cout << endl;
	}

	fin.close();
	return 0;
}

/*
3
29 13
++++++++++++++++++++++++++++ +
+....... + ....... + ........... +
++++....++++ + ... + ....++++... +
+.. + ........ + ... + .... + .. + ... +
+..++++ + ....++++ + ....++++... +
+........... + ... + .... + .. + ... +
+..++++++++++...++++++.. + ... +
+.. + .........++ + ........ + ... +
+.. + ......... + . + ..++ + ... + ... +
+.. + ......... + . + ..++ + ... + ... +
+... + ........ + . + ..++ + ... + ... +
+.... + ....... + . + ........ + ... +
++++++++++++++++++++++++++++ +
5 3
++++ +
+... +
++++ +
3 3
++ +
+. +
++ +
*/
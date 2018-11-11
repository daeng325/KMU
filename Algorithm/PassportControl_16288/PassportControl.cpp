#include <iostream>
#include <fstream>
using namespace std;

bool isPossible(int n, int k, int *incoming , int *port) {
	for (int i=0; i < n; i++) {
		int j = 0;
		while (j < k) {
			if (incoming[i] > port[j]) {
				port[j] = incoming[i];
				break;
			}
			j++;
		}
		if (j == k) {
			return false;
		}
	}
	return true;
}

void setIncoming(int n, int * arr, ifstream &fin) {
	for (int i = 0; i < n; i++) {
		int num;
		fin >> num;
		//if (fin.peek() == EOF)break;
		arr[i] = num;
	}
}

void setPort(int k, int *arr, ifstream &fin) {
	for (int i = 0; i < k; i++) {
		arr[i] = 0;
	}
}

int main() {
	ifstream fin;
	fin.open("input.txt");
	int numTestCase;
	fin >> numTestCase;

	for (int t = 0; t < numTestCase; t++) {
		int num_of_incoming, num_of_window;
		fin >> num_of_incoming >> num_of_window;
		if (fin.peek() == EOF)break;
		int *incoming = new int[num_of_incoming];
		int *port = new int[num_of_window];

		setIncoming(num_of_incoming, incoming, fin);
		setPort(num_of_window, port, fin);

		

		if (isPossible(num_of_incoming,num_of_window,incoming,port)) {
			cout << "YES" << endl;
		}
		else {
			cout << "NO" << endl;
		}

	/*	for (int i = 0; i < num_of_incoming; i++) {
			cout << incoming[i] << endl;
		}
		for (int i = 0; i < num_of_window; i++) {
			cout << port[i] << endl;
		}*/
	}
	return 0;
}
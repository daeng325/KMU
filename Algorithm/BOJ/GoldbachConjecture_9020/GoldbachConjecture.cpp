#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
using namespace std;

int is_prime(int n) {
	int sqrn = (int)sqrt(n);
	for (int i = 2; i <= sqrn; i++) {
		if (n%i == 0) {
			return false;
		}
	}
	return true;
}

void setVector(vector<int> &vec) {
	for (int n = 2; n < 10000; n++) {
		if (is_prime(n)) {
			vec.push_back(n);
		}
	}
}

void function(vector<int> &vec, int input_num) {
	int sub = 10000, num1, num2;
	for (int i = 0; i < vec.size(); i++) {
		for (int j = i; j < vec.size(); j++) {
			if (vec[i] + vec[j] == input_num) {
				/*if (sub > abs(vec[i] - vec[j])) {
					sub = abs(vec[i] - vec[j]);
				}
				cout << sub << endl;*/
				/*cout << vec[i] << " " << vec[j] << endl;*/
				num1 = vec[i];
				num2 = vec[j];

			}
		}
	}
	cout << num1 << " " << num2 << endl;
}

int main() {
	int numTestCase;
	cin >> numTestCase;

	vector<int> prime_num;
	setVector(prime_num);
	

	for (int i = 0; i < numTestCase; i++) {
		int input_num;
		cin >> input_num;

		function(prime_num,input_num);
	}

	prime_num.clear();
	return 0;
}
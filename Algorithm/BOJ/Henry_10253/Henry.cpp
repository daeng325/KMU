#include <iostream>
using namespace std;

int gcd(int num1, int num2) {
	if (num2 == 0) {
		return num1;
	}
	else{
		gcd(num2, num1%num2);
	}
}

void function(int a, int b) {

	int mod = b / a;
	int R = b % a;

	int xn = mod + 1;

	if (R == 0) {
		cout << mod << endl;
	}
	else {
		int bn = b * xn;
		int an = a * xn - b;
		int gcd_num = gcd(an, bn);
		function(an/gcd_num, bn/gcd_num);
	}
}

int main() {
	int T, num1, num2;
	cin >> T;
	for (int i = 0; i < T; i++) {
		cin >> num1 >> num2;
		function(num1,num2);
	}
	return 0;
}
#include <iostream>
#include <cmath>
using namespace std;

int main() {
	int numTestCase;
	cin >> numTestCase;
	for (int i = 0; i < numTestCase; i++) {
		long long n;
		cin >> n;

		int count;
		for (int j = 1; pow(2,j)*j <= n; j++) {
			count = j;
		}
		cout << count << endl;
	}
	return 0;
}
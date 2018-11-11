#include <iostream>
using namespace std;

int get_sheep_num(int a, int b, int n, int w) {
	if (a == b) {
		if (n*b == w && n % 2 == 0) {
			return n / 2;
		}
	}
	else {
		int x = (w - n * b) / (a - b);
		int R = (w - n * b) % (a - b);
		if (x < 1 || n - x < 1 || R != 0) {
			return -1;
		}
		else {
			return x;
		}
	}
}

int main() {
	int numTestCase;
	cin >> numTestCase;
	for (int i = 0; i < numTestCase; i++) {
		int sheep_per_day, goat_per_day, total_number, total_amount_food;
		cin >> sheep_per_day >> goat_per_day >> total_number >> total_amount_food;

		int sheep_num = get_sheep_num(sheep_per_day, goat_per_day, total_number, total_amount_food);
		int goat_num = total_number - sheep_num;
		if (sheep_num == -1) {
			cout << -1 << endl;
		}
		else {
			cout << sheep_num << " " << goat_num << endl;
		}

	}
	return 0;
}
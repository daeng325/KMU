#include <iostream>
using namespace std;

void function1(int **arr, int **check, int n, int m) {
	for (int i = 0; i < n; i++) {
		int idx1, idx2, max = -1;
		for (int j = 0; j < m; j++) {
			if (arr[i][j] > max) {
				max = arr[i][j];
				idx1 = i;
				idx2 = j;
			}
		}
		check[idx1][idx2] = 1;
	}
	for (int j = 0; j < m; j++) {
		int idx1, idx2, max = -1;
		for (int i = 0; i < n; i++) {
			if (arr[i][j] > max) {
				max = arr[i][j];
				idx1 = i;
				idx2 = j;
			}
		}
		check[idx1][idx2] = 1;
	}
}

 int main() {
	int numTestCase, n, m;
	cin >> numTestCase;

	for (int i = 0; i < numTestCase; i++) {
		cin >> n >> m;
		int **arr = new int*[n];
		int **check = new int*[n];

		for (int i = 0; i < n; i++)
		{
			arr[i] = new int[m];
		}
		for (int i = 0; i < n; i++)
		{
			check[i] = new int[m];
		}

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				int num;
				cin >> num;
				arr[i][j] = num;
			}
		}

		function1(arr, check, n, m);

		unsigned long sum = 0;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				if (check[i][j] != 1) {
					sum += arr[i][j];
				}
			}
		}
		cout << sum << endl;
	}
	return 0;
}

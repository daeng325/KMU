#include <iostream>
#include <queue>
using namespace std;

int function(int n, queue<int> &Q, int L, int w) {
	int time = 0, sum = 0;
	for (int i = 0; i < n; i++) {
		int num;
		cin >> num;

		while (1) {
			sum -= Q.front();
			Q.pop();
			if (sum+num<=L) {
				Q.push(num);
				sum += num;
				time++;
				break;
			}
			Q.push(0);
			time++;
		}
	}
	return time + w;
}

int main() {
	int T, n, w, L;
	cin >> T;

	for (int i = 0; i < T; i++) {
		cin >> n >> w >> L;
		queue <int> Q;

		for (int j = 0; j < w; j++) {
			Q.push(0);
		}

		int time = 0, sum = 0;
		for (int k = 0; k < n; k++) {
			int num;
			cin >> num;

			while (1) {
				sum -= Q.front();
				Q.pop();
				time++;
				if (sum + num <= L) {
					Q.push(num);
					sum += num;
					break;
				}
				else{
					Q.push(0);
				}
			}
		}
		int result =  time + w;
		cout << result << endl;
	}
	return 0;
}

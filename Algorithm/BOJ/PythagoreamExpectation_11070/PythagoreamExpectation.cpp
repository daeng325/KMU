#include <iostream>
#include <algorithm>
using namespace std;

void pushZero(int num_team, int *arr) {
	for (int j = 0; j < num_team; j++) {
		arr[j] = 0;
	}
}

void get_result(int *W, int *S, int *A, int num_team) {
	for (int j = 0; j < num_team; j++) {
		if (S[j] == 0 && A[j] == 0) {
			W[j] = 0;
		}
		else {
			W[j] = ((S[j] * S[j])*1000 / ((S[j] * S[j]) + (A[j] * A[j])));
		}
	}

	sort(W, W + num_team);

	int max = W[num_team - 1];
	int min = W[0];

	cout << max << endl;
	cout << min << endl;
}

void inputPlay(int m, int *S, int *A) {
	for (int j = 0; j < m; j++) {
		int teamA, teamB, scoreA, scoreB;
		cin >> teamA >> teamB >> scoreA >> scoreB;
		S[teamA - 1] += scoreA;
		A[teamA - 1] += scoreB;
		S[teamB - 1] += scoreB;
		A[teamB - 1] += scoreA;
	}
}


int main() {
	int numTestCast;
	cin >> numTestCast;
	for (int i = 0; i < numTestCast; i++) {
		int num_team, num_play;
		cin >> num_team >> num_play;

		int *S = new int[num_team + 1];
		int *A = new int[num_team + 1];

		pushZero(num_team, S);
		pushZero(num_team, A);

		inputPlay(num_play,S,A);

		int *W = new int[num_team + 1];

		get_result(W, S, A, num_team);
	}
	return 0;
}
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

bool compare2(pair<int, int> a, pair<int, int> b) {
	return a > b;
}

bool compare(pair<int, int> a, pair<int, int> b) {
	return a < b;
}

int main()
{
	int numTestCase;
	scanf("%d", &numTestCase);

	for (int t = 0; t < numTestCase; t++) {
		vector <pair<int, int> > pos;
		pos.push_back(pair<int, int>(-1, 0));
		int numOfCafe, x, y, m;

		scanf("%d", &numOfCafe);
		for (int j = 0; j < numOfCafe; j++) {
			scanf("%d", &x);
			scanf("%d", &y);
			pos.push_back(pair<int, int>(x, y));
		}
		sort(pos.begin(), pos.end(), compare);

		for (int i = 0; i < pos.size() - 1; i++) {
			//	int count = 0;
			if (pos[i].first != pos[i + 1].first && pos[i].second != pos[i + 1].second) {
				int j = i + 1;
				while (j+1 <pos.size() && pos[j].first == pos[j + 1].first) {
					//count++;
					j++;
				}
				sort(pos.begin() + i + 1, pos.begin() + j + 1, compare2);
			}
		}

		//k에 해당하는 카페의 위치 x,y 출력
		scanf("%d", &m);
		for (int i = 0; i < m; i++) {
			int num;
			scanf("%d", &num);
			cout << pos[num].first << " " << pos[num].second << endl;
		}
	}
	return 0;
}

/*
1
17
3 3
5 3
11 2
9 2
2 1
3 1
5 1
0 0
1 0
2 0
9 0
11 -1
9 -3
6 -1
7 -1
7 -3
5 -1
4 1 5 14 17
*/

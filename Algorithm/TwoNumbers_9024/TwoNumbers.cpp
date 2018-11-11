#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
/*
binary_search(begin, end, value)
탐색 대상이 되는 값이 존재하는지를 확인
들어있다면 true, 그렇지 않다면 false
*/
using namespace std;
int num[1000001] = { 0, };

int twoNumbers(int n, int K) {
	int count = 0;
	int min = 200000000;
	for (int i = 0; i<n - 1; i++)
	{
		int left = i + 1;
		int right = n - 1;

		while (left <= right)
		{
			int mid = (left + right) / 2;
			if (abs(K - (num[i] + num[mid]))<min)
			{
				min = abs(K - num[i] - num[mid]);
				count = 1;
			}
			else if (abs(K - (num[i] + num[mid])) == min)
				count++;
			if (K - (num[i] + num[mid])>0)
			{
				left = mid + 1;
			}
			else if (K - (num[i] + num[mid]) == 0)
			{
				break;
			}
			else
			{
				right = mid - 1;
			}

		}
	}
	return count;
}


//
//int twoNumbers(vector<int> &v, int n, int K) {
//	int count = 0;
//	for (int i = 0; i < v.size(); i++) {
//		if (binary_search(v.begin(), v.end(), K - v[i])) {
//			count++;
//			//cout << v[i] << endl;
//		}
//	}
//
//	if (count == 0) {
//		for (int i = 0; i < n; i++) {
//			int num = 0;//k보다 num 만큼 크거나 num만큼 작은수에 대한 결과값 찾기
//			for (int j = 0; j < v.size(); j++) {
//				if (binary_search(v.begin(), v.end(), (K + num) - v[j])) {
//					count++;
//				}
//				else if (binary_search(v.begin(), v.end(), (K - num) - v[j])) {
//					count++;
//				}
//			}
//			num++;
//		}
//		
//	}
//	
//	return count / 2;
//}

int main() {
	int numTestCase;
	scanf("%d", &numTestCase);

	for (int i = 0; i < numTestCase; i++) {
		int size_of_set, K;
		scanf("%d", &size_of_set);
		scanf("%d", &K);

		for (int i = 0; i <size_of_set; i++) {
			scanf("%d", &num[i]);
			
		}
		sort(num,num+size_of_set);

		cout << twoNumbers(size_of_set, K) << endl;
	}

	return 0;
}
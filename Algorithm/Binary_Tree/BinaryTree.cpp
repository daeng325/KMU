//#include <iostream>
//#include <cmath>
//#include <algorithm>
//using namespace std;
//
////typedef struct node *treePointer;
////typedef struct node {
////	int data;
////	treePointer leftChild, rightChild;
////}node;
////
////void post_order(treePointer ptr) {
////	if (ptr) {
////		post_order(ptr->leftChild);
////		post_order(ptr->rightChild);
////		cout << ptr->data << ' ';
////	}
////}
//
//
//int main() {
//	int height;
//	cin >> height;
//
//	int size = pow(2, height + 1) - 2;
//	int *arr = new int[size+2];
//	for (int i = 0; i < size; i++) {
//		int weight;
//		cin >> weight;
//		arr[i] = weight;
//	}
//	for (int i = size - 2; i >= 0; i= i-2) {
//		if (i == 0) {
//			if (size == 2) {
//				int res = max(arr[i], arr[i + 1]);
//				arr[i] = res;
//				arr[i + 1] = res;
//			}
//			else {
//				int res = max(arr[2] + arr[0], arr[4] + arr[1]);
//				int dif1 = res - (arr[2] + arr[0]);
//				int dif2 = res - (arr[4] + arr[1]);
//				arr[0] += dif1;
//				arr[1] += dif2;
//			}
//		}
//		else if (arr[i] != arr[i + 1]) {
//			int res = max(arr[i], arr[i + 1]);
//			arr[i] = res;
//			arr[i + 1] = res;
//		}
//	}
//
//
//	int sum = 0;
//	for (int i = 0; i < size; i++) {
//		sum += arr[i];
//	}
//
//	cout << sum << endl;
//	return 0;
//}
//










//#include <iostream>
//#include <cstdio>
//#define SIZE (1<<21)
//#define MAX(a,b) (a>b? a:b)
//using namespace std;
//
//int main(){
//	int num,height,size,left,right;
//	//cin>>height;
//	scanf("%d", &height);
//	size = 1 << (height + 1);
//	int tree[SIZE] = {0}, path[SIZE] = {0}, sum[SIZE]={0};
//	for(int i=0; i<size; i++){
//		//cin>>num;
//		scanf("%d", &num);
//		tree[i] = num;
//	}
//	for(int i=1<<height; --i>0;){
//		left = i<<1; right=left+1;
//		path[i] = MAX(path[left]+tree[left] , path[right]+tree[right]);
//		sum[i] = sum[left] + sum[right] + (path[i]-path[left]) + (path[i]-path[right]);
//	}
//	cout<<sum[1]<<endl;
//	return 0;
//}

//#include<iostream>
//#include<algorithm>
//using namespace std;
//
//int input[10000000], M[10000000], DP[10000000];
//
//int main()
//{
//	int numTestCase;
//	cin >> numTestCase;
//	for (int i = 0; i < numTestCase; i++) {
//		int N;
//		cin >> N;
//		int maxi = 1;
//		for (int i = 0; i <= N; i++) {
//			maxi *= 2;
//		}
//
//		for (int i = 2; i < maxi; i++) {
//			int num;
//			cin >> num;
//			input[i] = num;
//		}
//
//		maxi /= 2;
//		maxi -= 1;
//
//		int Ans = 0;
//		for (int i = maxi; i > 0; i--) {
//			M[i] = max(M[i + i] + input[i + i], M[i + i + 1] + input[i + i + 1]);
//			DP[i] = M[i] - M[i + i] + M[i] - M[i + i + 1];
//			Ans += DP[i];
//		}
//		cout << Ans << endl;
//	}
//	return 0;
//}


//#include <iostream>
//#include <cstdlib>
//#include <algorithm>
//using namespace std;
//
//int a[21][1 << 21];
//
//int main() {
//	int height;
//	cin >> height;
//
//	int sum = 0;
//	for (int i = 1; i <= height; i++)
//		for (int j = 0; j < 1 << i; j++) {
//			cin >> a[i][j];
//			sum += a[i][j];
//		}
//
//	for (int i = height; i > 0; i--)
//		for (int j = 0; j < 1 << i - 1; j++) {
//			a[i - 1][j] += max(a[i][j * 2], a[i][j * 2 + 1]);
//			sum += abs(a[i][j * 2] - a[i][j * 2 + 1]);
//		}
//	cout << sum << endl;
//}

#include <iostream>
#include <algorithm>
using namespace std;
int arr[1 << 21];


int main() {
	int numTestCase;
	scanf("%d", &numTestCase);
	for (int i = 0; i < numTestCase; i++) {
		int k, ans = 0;
		scanf("%d", &k);
		for (int i = 2; i < (1 << (k + 1)); i++) {
			scanf("%d", &arr[i]);
			ans += arr[i];
		}

		for (int i = k; i > 0; i--) {
			for (int j = (1 << i); j < (1 << (i + 1)); j += 2) {
				ans += abs(arr[j] - arr[j + 1]);
				arr[j / 2] += max(arr[j], arr[j + 1]);
			}
		}

		cout << ans << endl;
	}
	return 0;
}
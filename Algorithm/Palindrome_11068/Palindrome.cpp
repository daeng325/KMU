#include <iostream>
#include <vector>
using namespace std;

int checkPalindrome(vector<int> &vec, int first, int last)
{
	if (last <= first)
		return 1;
	else if (vec[first] != vec[last])
		return 0;
	else
		return checkPalindrome(vec, first + 1, last - 1);
}

void baseConversion(int num, int base, vector<int> &vec) {

	if (num >= base) {
		baseConversion(num / base, base, vec);
	}
	vec.push_back(num%base);

}

int main() {

	int numTestcase, num;
	cin >> numTestcase;
	
	for (int i = 0; i < numTestcase; i++) {
		cin >> num;
		int isPalindrome = 0;
		vector<int> vec;

		for (int j = 2; j <= 64; j++) {
			baseConversion(num,j,vec);
			if (checkPalindrome(vec, 0, vec.size() - 1)) {
				isPalindrome = 1;
				vec.clear();
				break;
			}
			vec.clear();
		}

		cout << isPalindrome << endl;
		
	}
	return 0;
}

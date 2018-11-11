#include <iostream>
#include <fstream>
using namespace std;

int Fibonacci(int num) {
	if (num <=1){
		return num;
	}
	else {
		return Fibonacci(num - 1) + Fibonacci(num - 2);
	}
}

int Factorial(int num) {
	if (num<=1) {
		return 1;
	}
	else {
		return num * Factorial(num - 1);
	}
}

int get_size(char *str) {
	int size = 0;
	for (int i = 0; i < 1001; i++) {
		if (str[i] == NULL) {
			break;
		}
		size++;
	}
	return size;
}

int isPalindrome(char str[], int start, int end) {
	if (start >= end) {
		return 1;
	}
	else if (str[start] != str[end]) {
		return 0;
	}
	else {
		return isPalindrome(str, start + 1, end - 1);
	}
}


int main() {
	ifstream fin;
	fin.open("input.txt");
	int numTestCase;
	fin >> numTestCase;

	for (int i = 0; i < numTestCase; i++) {
		char *str = new char [1001];
		fin >> str;

		int size = get_size(str);

		cout << isPalindrome(str, 0, size - 1) << endl;
	}

	fin.close();

}
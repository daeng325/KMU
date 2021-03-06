// IsPrime.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <math.h>
#include <iostream>

int is_prime(int n) {
	int sqrn = (int)sqrt(n);
	for (int i = 2; i <= sqrn; i++) {
		if (n%i == 0) {
			return false;
		}
	}
	return true;
}


int main(int argc, char * argv[])
{
	/*bool prime = is_prime(17);
	std::cout << prime << std::endl;*/

	/*if (argc < 2) {
		std::cout << "Error!" << std::endl;
		return 0;
	}*/

	//int n = atoi(argv[1]);
	int n = 30;
	if (n < 2) {
		std::cout << "Error!" << std::endl;
		return 0;
	}

	int *parray;
	parray = new int[n + 1];

	if (parray == 0) {
		std::cout << "Memory Allocation Error!" << std::endl;
		return 0;
	}

	memset(parray, 0, sizeof(int)*(n + 1));

	int i, j;
	for (i = 2; i < n + 1; i++) {
		if (parray[i] == 1) continue;

		j = i;
		while ((j += i) <= n) {
			parray[j] = 1;
		}
	}

	for (i = 2; i < n + 1; i++) {
		if (parray[i] == 0) {
			std::cout << i;
			std::cout << " ";
		}
	}
	std::cout << std::endl;

	delete[] parray;

    return 0;
}


// GCD.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "iostream"


int get_gcd(int u, int v) {
	int t;
	while (u) {
		if (v > u) {
			t = v;
			v = u;
			u = t;
		}
		u = u - v;
	}
	return v;
}

int get_gcd_mod(int u, int v) {
	int t;
	while (u) {
		t = u % v;
		v = u;
		u = t;
	}
	return v;
}

int get_gcd_recursion(int u, int v) {
	if (v == 0) {
		return u;
	}
	else {
		return get_gcd_recursion(v, u%v);
	}
}

int main()
{
	int u = 280;
	int v = 30;

	//int res = get_gcd(u,v);

	//int res1 = get_gcd_mod(u, v);
	int res2 = get_gcd_recursion(u, v);

	//std::cout << res1 << std::endl;
	std::cout << res2 << std::endl;


	
	
	return 0;
}


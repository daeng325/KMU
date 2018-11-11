#include <iostream>
#include <cmath>

int result;

int Turret(int x1, int y1, int r1, int x2, int y2, int r2) {
	double length;//µÎ Á¡ÀÇ °Å¸®

	length = sqrt(pow(x1 - y1,2.0) + pow(x2 - y2,2.0));

	if (x1 == x2 && y1 == y2) {
		if (r1 == r2) {
			result = -1;
		}
		else 
			result= 0;
	}

	else {
		if (length > abs(r1 - r2) && length < r1 + r2) {
			result = 2;
		}
		else if (length == r1 + r2 || length == abs(r1 - r2)) {
			result = 1;
		}
		else {
			result = 0;
		}

	}

	return 0;
}

int output() {
	std::cout << result << std::endl;
	return 0;
}

int main() {

	int T, x1, y1, r1, x2, y2, r2;
	

	std::cin >> T;

	
	for (int i = 0; i < T; i++) {
		std::cin >> x1 >> y1 >> r1 >> x2 >> y2 >> r2;
		Turret(x1, y1, r1, x2, y2, r2);
		output();
	}

	return 0;
}

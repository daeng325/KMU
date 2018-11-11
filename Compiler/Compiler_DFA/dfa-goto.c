///*
//DFA by goto statement.
//
//Regular expression for "a+b+c+d".
//*/
//#include <stdio.h>
//
//#define FAIL 0;
//#define OKAY 1;
//
//int ch;
//
//int dfa()
//{
//	ch = getchar();
//S:
//	if (ch == 'a') {
//		ch = getchar();
//		goto A;
//	}
//	else return FAIL;
//A:
//	if (ch == 'a') {
//		ch = getchar();
//		goto A;
//	}
//	else if (ch == 'b') {
//		ch = getchar();
//		goto B;
//	}
//	else return FAIL;
//B:
//	if (ch == 'b') {
//		ch = getchar();
//		goto B;
//	}
//	else if (ch == 'c') {
//		ch = getchar();
//		goto C;
//	}
//	else return FAIL;
//C:
//	if (ch == 'c') {
//		ch = getchar();
//		goto C;
//	}
//	else if (ch == 'd') {
//		ch = getchar();
//		goto F;
//	}
//	else return FAIL;
//F:
//	if (ch == '\n') {
//		return OKAY;
//	}
//	else return FAIL;
//	//final state에서 끝나지 않면 fail인 거고..
//	//hard coding 되어있음. 단점이 있다.
//
//}
//
//main()
//{
//	if (dfa())
//		puts("OK");
//	else puts("FAIL");
//}

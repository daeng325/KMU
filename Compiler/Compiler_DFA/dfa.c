/*
Simple DFA recognition program.
DFA -- (1+01)*00(0+1)*
*/
#include <stdio.h>

#define STATES	3	/* number of states */
#define SYMBOLS	2	/* number of symbols */

/*
State Tranisition Table in p86. 예제 33.14
*/
int ST_TAB[STATES][SYMBOLS] = {
	/* state p */{ 1, 0 },
	/* state q */{ 2, 0 },
	/* state r */{ 2, 2 }
};
//상태전이표를 2차원 배열로! final state표시를 그대로 구현하자!
//배열의 값만 바꿔주면 다른 오토마타를 표현할 수 있다!
//data file로 줘서 loading을 수행하면 더 좋겠지.

void put_table(int tab[][SYMBOLS], int m, int n)
{
	int i;

	puts("State Transition Table");
	puts("\t0\t1");
	for (i = 0; i < m; i++)
		printf("%c\t%c\t%c\n",
			'A' + i, 'A' + tab[i][0], 'A' + tab[i][1]);
	puts("Final states = { C }");
}

void main()
{
	int ch;	/* input symbol */
	int cur_state = 0;	/* start state */
	//0번 state에서 input이...?
	//1또는 01이 반복되다가 00오고나서 0또는 1이 오는 유형
	// (1+01)*00(0+1)*

	put_table(ST_TAB, STATES, SYMBOLS);

	ch = getchar();
	while (ch != '\n') {
		cur_state = ST_TAB[cur_state][ch - '0']; 
		if (cur_state < 0) { puts("FAIL"); return; } //아무데도 갈 곳이 없으면 minus 값을 준다.
		ch = getchar();
	}

	if (cur_state == 2) puts("OK");	/* final states = { 2 } */
	//final state에 속하냐 속하지 않느냐.만 수정해주면 된다..!
	else puts("FAIL");
}

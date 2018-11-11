/*
Simple DFA recognition program.
DFA -- (1+01)*00(0+1)*
*/
#include <stdio.h>

#define STATES	3	/* number of states */
#define SYMBOLS	2	/* number of symbols */

/*
State Tranisition Table in p86. ���� 33.14
*/
int ST_TAB[STATES][SYMBOLS] = {
	/* state p */{ 1, 0 },
	/* state q */{ 2, 0 },
	/* state r */{ 2, 2 }
};
//��������ǥ�� 2���� �迭��! final stateǥ�ø� �״�� ��������!
//�迭�� ���� �ٲ��ָ� �ٸ� ���丶Ÿ�� ǥ���� �� �ִ�!
//data file�� �༭ loading�� �����ϸ� �� ������.

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
	//0�� state���� input��...?
	//1�Ǵ� 01�� �ݺ��Ǵٰ� 00������ 0�Ǵ� 1�� ���� ����
	// (1+01)*00(0+1)*

	put_table(ST_TAB, STATES, SYMBOLS);

	ch = getchar();
	while (ch != '\n') {
		cur_state = ST_TAB[cur_state][ch - '0']; 
		if (cur_state < 0) { puts("FAIL"); return; } //�ƹ����� �� ���� ������ minus ���� �ش�.
		ch = getchar();
	}

	if (cur_state == 2) puts("OK");	/* final states = { 2 } */
	//final state�� ���ϳ� ������ �ʴ���.�� �������ָ� �ȴ�..!
	else puts("FAIL");
}

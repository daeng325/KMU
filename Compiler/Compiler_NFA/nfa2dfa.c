/*
NFA --> DFA conversion program

국민대학교 컴퓨터공학부 강승식
Email: sskang@kookmin.ac.kr
컴파일러 과목 카페 -- http://cafe.daum.net/sskang-compiler
홈페이지 -- http://nlp.kookmin.ac.kr/
*/
#include <stdio.h>
#include <string.h>

#define STATES	256
#define SYMBOLS	20

int N_symbols;	/* number of input symbols */
int NFA_states;	/* number of NFA states */
char *NFAtab[STATES][SYMBOLS];

int DFA_states;	/* number of DFA states */
int DFAtab[STATES][SYMBOLS];

/*
Print state-transition table.
State names: 'A', 'B', 'C', ...
*/
void put_dfa_table(
	int tab[][SYMBOLS],	/* DFA table */
	int nstates,	/* number of states */
	int nsymbols)	/* number of input symbols */
{
	int i, j;

	puts("DFA: STATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("    | ");
	for (i = 0; i < nsymbols; i++) printf("\t%c", '0' + i);

	printf("\n----+--");
	for (i = 0; i < nsymbols; i++) printf("-------");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c | ", 'A' + i);	/* state */
		for (j = 0; j < nsymbols; j++)
			printf("\t%c", 'A' + tab[i][j]);
		printf("\n");
	}
}

void put_nfa_table(
	char *tab[][SYMBOLS],	/* NFA table */
	int nstates,	/* number of states */
	int nsymbols)	/* number of input symbols */
{
	int i, j;

	puts("NFA: STATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("    | ");
	for (i = 0; i < nsymbols; i++) printf("\t%c", '0' + i);

	printf("\n----+--");
	for (i = 0; i < nsymbols; i++) printf("-------");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c |", '0' + i);	/* state */
		for (j = 0; j < nsymbols; j++)
			printf("\t%s", tab[i][j]);
		printf("\n");
	}
	printf("\n");
}

/*
Initialize NFA table.
*/
void init_NFA_table()
{
	/*
	Sample NFA table -- 1번째

	NFAtab[0][0] = "01";
	NFAtab[0][1] = "0";
	NFAtab[1][0] = "";
	NFAtab[1][1] = "01";

	NFA_states = 2;
	DFA_states = 0;
	N_symbols = 2;
	// Final states = { 1 } 로 해서 기능 추가
	*/
	/*
	Sample NFA table -- 2번째
	*/
	NFAtab[0][0] = "12";
	NFAtab[0][1] = "13";
	NFAtab[1][0] = "12";
	NFAtab[1][1] = "13";
	NFAtab[2][0] = "4";
	NFAtab[2][1] = "";
	NFAtab[3][0] = "";
	NFAtab[3][1] = "4";
	NFAtab[4][0] = "4";
	NFAtab[4][1] = "4";

	NFA_states = 5;
	DFA_states = 0;
	N_symbols = 2;
	// Final states = { 3, 4 } 로 해서 기능 추가
}

/*
String 't' is merged into 's' in an alphabetical order.
*/
void string_merge(char *s, char *t)
{
	char temp[STATES], *r = temp, *p = s;

	while (*p && *t) {
		if (*p == *t) {
			*r++ = *p++; t++;
		}
		else if (*p < *t) {
			*r++ = *p++;
		}
		else
			*r++ = *t++;
	}
	*r = '\0';

	if (*p) strcat(r, p);
	else if (*t) strcat(r, t);

	strcpy(s, temp);
}

/*
Get next-state string for current-state string.
(state 스트링이므로 각 state에 대해 nextstate를 merge)
*/
void get_next_state(char *nextstates, char *cur_states,
	char *nfa[STATES][SYMBOLS], int n_nfa, int symbol)
{
	int i;
	char temp[STATES];

	temp[0] = '\0';
	for (i = 0; i < strlen(cur_states); i++)
		string_merge(temp, nfa[cur_states[i] - '0'][symbol]);
	strcpy(nextstates, temp);
}

/*
statename 테이블에서 'state'를 찾아 index를 return.
'state'가 테이블에 없으면 끝에 추가하고 index를 return.
*/
int state_index(char *state, char statename[][STATES], int *pn)
{
	int i;

	if (!*state) return -1;	/* no next state */

	for (i = 0; i < *pn; i++)
		if (!strcmp(state, statename[i])) return i;

	strcpy(statename[i], state);	/* new state-name */
	return (*pn)++;
}

/*
Convert NFA table to DFA table.
Method:
0. state-name이 스트링이므로 statename 테이블 이용
'n' -- statename[]에 등록된 state 개수
1. DFA table의 entry 개수를 1로 초기화 및 statename에 추가
2. statename[i]의 각 symbol들에 대해 nextstate 계산
3. nextstate가 스트링이므로 statename의 index를 DFA에 넣음
Return value: number of DFA states.
*/
int nfa_to_dfa(char *nfa[STATES][SYMBOLS], int n_nfa,
	int n_sym, int dfa[][SYMBOLS])
{
	char statename[STATES][STATES];
	int i = 0;	/* current index of DFA */
	int n = 1;	/* number of DFA states */

	char nextstate[STATES];
	int j;

	strcpy(statename[0], "0");	/* start state */

	for (i = 0; i < n; i++) {	/* for each DFA state */
		for (j = 0; j < n_sym; j++) {	/* for each input symbol */
			get_next_state(nextstate, statename[i], nfa, n_nfa, j);
			dfa[i][j] = state_index(nextstate, statename, &n);
		}
	}

	return n;	/* number of DFA states */
}

void main()
{
	init_NFA_table();
	put_nfa_table(NFAtab, NFA_states, N_symbols);

	DFA_states = nfa_to_dfa(NFAtab, NFA_states, N_symbols, DFAtab);
	put_dfa_table(DFAtab, DFA_states, N_symbols);
}

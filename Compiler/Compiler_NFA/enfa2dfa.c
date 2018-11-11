/*
Epsilon-NFA --> DFA conversion program

국민대학교 컴퓨터공학부 강승식
Email: sskang@kookmin.ac.kr
컴파일러 과목 카페 -- http://cafe.daum.net/sskang-compiler
홈페이지 -- http://nlp.kookmin.ac.kr/
*/
#include <stdio.h>
#include <string.h>

#define STATES	99
#define SYMBOLS	20

int N_symbols;	/* number of input symbols */
int N_NFA_states;	/* number of NFA states */
char *NFAtab[STATES][SYMBOLS];
//nfa table.. next state가 2개 이상이기 때문에 char* 로 선언.
char *NFA_finals;	/* NFA final states */

int N_DFA_states;	/* number of DFA states */
int DFAtab[STATES][SYMBOLS];
char DFA_finals[STATES + 1];	/* NFA final states */

char StateName[STATES][STATES + 1];	/* state name table */
char Eclosure[STATES][STATES + 1];	/* epsilon closure for each state */

									/*
									Print state-transition table.
									State names: 'A', 'B', 'C', ...
									*/
void print_nfa_table(
	char *tab[][SYMBOLS],	/* DFA table */
	int nstates,	/* number of states */
	int nsymbols,	/* number of input symbols */
	char *finals)	/* final states */
{
	int i, j;

	puts("\nNFA: STATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf("  %-6c", '0' + i);
	printf("  e\n");	/* epsilon */

	printf("-----+--");
	for (i = 0; i < nsymbols + 1; i++) printf("-------");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c  | ", '0' + i);	/* state */
		for (j = 0; j < nsymbols + 1; j++)
			printf("  %-6s", tab[i][j]);
		printf("\n");
	}
	printf("Final states = %s\n", finals);
}

/*
Print state-transition table.
State names: 'A', 'B', 'C', ...
*/
void print_dfa_table(
	int tab[][SYMBOLS],	/* DFA table */
	int nstates,	/* number of states */
	int nsymbols,	/* number of input symbols */
	char *finals)	/* final states */
{
	int i, j;

	puts("\nDFA: STATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf("  %c  ", '0' + i);

	printf("\n-----+--");
	for (i = 0; i < nsymbols; i++) printf("-----");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c  | ", 'A' + i);	/* state */
		for (j = 0; j < nsymbols; j++)
			printf("  %c  ", tab[i][j]);
		printf("\n");
	}
	printf("Final states = %s\n", finals);
}

/*
Initialize NFA table.
*/
void load_NFA_table()
{
	/*
	epsilon-NFA table for ex.24 at p.82
	Last input symbol is an epsilon.
	Input symbols : 0(a), 1(b), 2(epsilon)

	NFAtab[0][0] = "0";
	NFAtab[0][1] = "";
	NFAtab[0][2] = "13";
	NFAtab[1][0] = "2";
	NFAtab[1][1] = "";
	NFAtab[1][2] = "";
	NFAtab[2][0] = "";
	NFAtab[2][1] = "2";
	NFAtab[2][2] = "3";
	NFAtab[3][0] = "3";
	NFAtab[3][1] = "";
	NFAtab[3][2] = "";

	N_symbols = 2;
	N_NFA_states = 4;
	NFA_finals = "3";
	N_DFA_states = 0;
	*/

	/*
	epsilon-NFA table for ex.25 at p.82-83
	Last input symbol is an epsilon.
	Input symbols : 0(a), 1(b), 2(epsilon)

	NFAtab[0][0] = "1";
	NFAtab[0][1] = "";
	NFAtab[0][2] = "";
	NFAtab[0][3] = "2";
	NFAtab[1][0] = "";
	NFAtab[1][1] = "3";
	NFAtab[1][2] = "";
	NFAtab[1][3] = "";
	NFAtab[2][0] = "";
	NFAtab[2][1] = "";
	NFAtab[2][2] = "2";
	NFAtab[2][3] = "3";
	NFAtab[3][0] = "";
	NFAtab[3][1] = "";
	NFAtab[3][2] = "";
	NFAtab[3][3] = "";

	N_symbols = 3;
	N_NFA_states = 4;
	NFA_finals = "23";
	N_DFA_states = 0;
	*/

	/*
	Epsilon-NFA table for TEST.
	Last input symbol is an epsilon.
	Input symbols : 0(a), 1(b), 2(epsilon)
	*/
	NFAtab[0][0] = "01";
	NFAtab[0][1] = "0";
	NFAtab[0][2] = "";
	NFAtab[1][0] = "13";
	NFAtab[1][1] = "2";
	NFAtab[1][2] = "3";
	NFAtab[2][0] = "2";
	NFAtab[2][1] = "1";
	NFAtab[2][2] = "2";
	NFAtab[3][0] = "";
	NFAtab[3][1] = "1";
	NFAtab[3][2] = "";

	N_symbols = 2;
	N_NFA_states = 4;
	NFA_finals = "3";
	N_DFA_states = 0;
}

/*
String 't' is merged into 's' in an alphabetical order.

Return value: number of items that are added to 's'.
*/
int string_merge(char *s, char *t) //합집합 계산!! 

{
	int n = 0;
	char temp[STATES + 1], *r = temp, *p = s;

	while (*p && *t) {
		if (*p == *t) {
			*r++ = *p++; t++;
		}
		else if (*p < *t) {
			*r++ = *p++;
		}
		else {
			*r++ = *t++;
			n++;	/* an item is added to 's' */
		}
	}
	*r = '\0';

	if (*t) {
		strcat(r, t);
		n += strlen(t);
	}
	else if (*p) strcat(r, p);

	strcpy(s, temp);

	return n;
}

/*
Get next-state string for current-state string.
(state 스트링이므로 각 state에 대해 nextstate를 merge)
*/
void get_next_state_NFA(char *nextstates, char *cur_states,
	char *nfa[STATES][SYMBOLS], int symbol)
{
	int i;
	char temp[STATES + 1];

	temp[0] = '\0';
	for (i = 0; i < strlen(cur_states); i++)
		string_merge(temp, nfa[cur_states[i] - '0'][symbol]);
	strcpy(nextstates, temp);
}

/*
StateName 테이블 'stnt'에서 'state'를 찾아 index를 return.
'state'가 테이블에 없으면 끝에 추가하고 index를 return.
*/
int state_index(char *state, char stnt[][STATES + 1], int *pn)
{
	int i;

	if (!*state) return -1;	/* no next state */

	for (i = 0; i < *pn; i++)
		if (!strcmp(state, stnt[i])) return i;

	strcpy(stnt[i], state);	/* new state-name */
	return (*pn)++;
}

/*
'epstates': epsilon-accessible states for 'state'.
*/
void get_ep_states(int state, char *epstates,
	char *nfa[][SYMBOLS], int n_sym)
{
	int i, n = 0;	/* 'n': number of added states */

					/* directly epsilon-accessible states */
	strcpy(epstates, nfa[state][n_sym]);

	do {	/* get all indirectly epsilon-accessible states */
		for (i = 0; i < strlen(epstates); i++)
			n = string_merge(epstates, nfa[epstates[i] - '0'][n_sym]);
	} while (n);	/* 더 이상 추가되는 state가 없을 때까지 */
}

/*
Initialize 'Eclosure[]' for each NFA state.
<주의> 'Eclosure[i]'에 자기 state는 제외하였음!
*/
void init_Eclosure(char eclosure[][STATES + 1],
	char *nfa[][SYMBOLS], int n_nfa, int n_sym)
{
	int i;

	printf("\nEpsilon-accessible states:\n");
	for (i = 0; i < n_nfa; i++) {
		get_ep_states(i, eclosure[i], nfa, n_sym);
		printf("    state %d : [%s]\n", i, eclosure[i]);
	} printf("\n");
}

/*
Epsilon closure of 'states' is 'epstates'.
*/
void e_closure(char *epstates, char *states, char eclosure[][STATES + 1])
{
	int i;

	strcpy(epstates, states);
	for (i = 0; i < strlen(states); i++)
		string_merge(epstates, eclosure[states[i] - '0']);
}

/*
Convert NFA table to DFA table.
Method:
0. state-name이 스트링이므로 StateName 테이블 이용
'n' -- StateName[]에 등록된 state 개수
1. DFA table의 entry 개수를 1로 초기화 및 StateName에 추가
2. StateName[i]의 각 symbol들에 대해 nextstate 계산
3. nextstate가 스트링이므로 StateName의 index를 DFA에 넣음
Return value: number of DFA states.
*/
int nfa_to_dfa(char *nfa[][SYMBOLS], int n_nfa,
	int n_sym, int dfa[][SYMBOLS])
{
	int i = 0;	/* current index of DFA */
	int n = 1;	/* number of DFA states */

	char nextstate[STATES + 1];
	char temp[STATES + 1];	/* epsilon closure */
	int j;

	init_Eclosure(Eclosure, nfa, n_nfa, n_sym);
	//중요!!

	e_closure(temp, "0", Eclosure);
	//start. 시작기호....
	strcpy(StateName[0], temp);	/* initialize start state */

	printf("Epsilon-NFA to DFA conversion\n");
	for (i = 0; i < n; i++) {	/* for each DFA state */
		for (j = 0; j < n_sym; j++) {	/* for each input symbol */ //인풋심볼 갯수 만큼 다음 심볼을 계산하라...?
			//합집합 하는 함수...string merge!!
			get_next_state_NFA(nextstate, StateName[i], nfa, j); //next state를 가져옴
			e_closure(temp, nextstate, Eclosure);//가져온 것들에 다 epsilon+closure... //merge도 해줘야 함!
			dfa[i][j] = state_index(temp, StateName, &n);
			printf("    state %d(%4s) : %d --> state %2d(%4s)\n",
				i, StateName[i], j, dfa[i][j], temp);
			dfa[i][j] += 'A';	/* 0/1/2/... --> 'A/B/C/...' */
		}
	}

	return n;	/* number of DFA states */
}

/*
NFA의 final state가 하나라도 포함된 모든 state가 DFA의 final state임.
*/
void get_DFA_finals(
	char *dfinals,	/* DFA final states */
	char *nfinals,	/* NFA final states */
	char stnt[][STATES + 1],	/* state-name table */
	int n_dfa)	/* number of DFA states */
{
	int i, j, k = 0, n = strlen(nfinals);

	for (i = 0; i < n_dfa; i++) {
		for (j = 0; j < n; j++) {
			if (strchr(stnt[i], nfinals[j])) {
				dfinals[k++] = i + 'A';
				break;
			}
		}
	}
	dfinals[k] = '\0';
}

void main()
{
	load_NFA_table(); //epsilon-nfa
	print_nfa_table(NFAtab, N_NFA_states, N_symbols, NFA_finals);

	N_DFA_states = nfa_to_dfa(NFAtab, N_NFA_states, N_symbols, DFAtab); //elsilon-nfa -> dfa
	get_DFA_finals(DFA_finals, NFA_finals, StateName, N_DFA_states); //final state 수집하는 것. nfa2dfa는 적당히 대충 하느라 이런 게 없었대

	print_dfa_table(DFAtab, N_DFA_states, N_symbols, DFA_finals);
}

/*
주어진 문법에 의해 입력 스트링을 인식하는 프로그램 실습

- (ba)*a 를 기술하는 문법을 구현

- 이 프로그램에서 구현된 문법으로 인식되는 언어: (ba)*a

G = ({O, E}, {a, b}, P, O),
P: O -> a | bE
E -> aO

- 입력: terminal string
- 출력: (ba)*a 유형이 입력되면 인식 '성공(OK)', 그 외 다른 스트링은 인식 '실패(FAIL)'

국민대학교 컴퓨터공학부 강승식
*/
#include <stdio.h>

#define END '\n'
char ch;
int errflag = 0;

void error()
{
	if (!errflag) {
		puts("FAIL");
		errflag = 1;
	}
}

void pa() //terminal a
{
	if (ch == 'a')
		ch = getchar();
	else error();
}

void pb() //terminal b
{
	if (ch == 'b')
		ch = getchar();
	else error();
}

void pO();
void pE() //non terminal E
{
	if (ch == 'a') {
		pa(); pO();
	}
	else error();
}

void pO() //non terminal O
{
	if (ch == 'a') {
		pa();
	}
	else if (ch == 'b') {
		pb(); pE();
	}
	else error();
}

print_rules()
{
	puts("Production Rules:");
	puts("\tO -> bE");
	puts("\tO -> a");
	puts("\tE -> aO");
}

//goto는 final automata만 할 수 있다고??
//뭐...뭐래는 거야...
//오토마타 구현하는 것과 마찬가지로 이거에 맞는 게 들어오면 ok, 아니면 fail

void main()
{
	print_rules();	// production rules
	printf("Input string(ex. babaa) = ");

	ch = getchar();	// input string: "babaa"
	pO();
	if (!errflag && ch == END) puts("OK");
	else error();
}

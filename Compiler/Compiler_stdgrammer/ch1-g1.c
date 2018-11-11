/*
�־��� ������ ���� �Է� ��Ʈ���� �ν��ϴ� ���α׷� �ǽ�

- (ba)*a �� ����ϴ� ������ ����

- �� ���α׷����� ������ �������� �νĵǴ� ���: (ba)*a

G = ({O, E}, {a, b}, P, O),
P: O -> a | bE
E -> aO

- �Է�: terminal string
- ���: (ba)*a ������ �ԷµǸ� �ν� '����(OK)', �� �� �ٸ� ��Ʈ���� �ν� '����(FAIL)'

���δ��б� ��ǻ�Ͱ��к� ���½�
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

//goto�� final automata�� �� �� �ִٰ�??
//��...������ �ž�...
//���丶Ÿ �����ϴ� �Ͱ� ���������� �̰ſ� �´� �� ������ ok, �ƴϸ� fail

void main()
{
	print_rules();	// production rules
	printf("Input string(ex. babaa) = ");

	ch = getchar();	// input string: "babaa"
	pO();
	if (!errflag && ch == END) puts("OK");
	else error();
}
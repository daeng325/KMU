/*
LGS(Language Generating System) for CFG.
CFG should be described in 'expand()'.

<실습> 아래 프로그램은 어떤 문법으로 기술되는 스트링들을 생성하는가?
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX 999

int top = 0;
char *stack[MAX];

void push(char *s)
{
	if (top >= MAX - 1) {
		puts("STACK OVERFLOW!");
		return;
	}

	/*
	printf("STACK[%d] = %s\n", top, s);
	*/
	stack[top] = (char *)malloc(strlen(s) + 1);
	strcpy(stack[top++], s);
}

char *pop()
{
	return stack[--top];
}

int is_stack_empty()
{
	return (top <= 0);
}

/*
maximum 'n' strings are generated.
*/
int expand(char *s, int n)
{
	char *p = s;
	char *temp, *pt;	/* temp -- expanded string */

	temp = (char *)malloc(strlen(s) + 99);
	pt = temp;

	while (*p) {
		if (isupper(*p)) break;
		*pt++ = *p++;
	}	/* '*p' is a leftmost nonterminal */
	if (*p == '\0') {	/* no nonterminal */
		printf("STRING: %s\n", s);
		return n - 1;
	}

	switch (*p) {
	case 'S':	/* S --> aAb */
		*pt = '\0';
		strcat(pt, "aAb");
		strcat(pt, p + 1);
		push(temp);
		break;
	case 'A':	/* A --> aS, A --> b */
		*pt = '\0';
		strcat(pt, "aS");
		strcat(pt, p + 1);
		push(temp);

		*pt = '\0';
		strcat(pt, "b");
		strcat(pt, p + 1);
		push(temp);
		break;
	}

	free(temp);
	return n;
}

print_rules()
{
	puts("Production Rules:");
	puts("\tS --> aAb");
	puts("\tA --> aS");
	puts("\tA --> b");
}

//무한대로 생성하기 때문에 다 할 수 있는 것은 아님...참고.

void main()
{
	char *sform;
	int n = 20;	/* max. 'n' strings are generated */

	print_rules();	// production rules

	push("S");	// initialize stack by start symbol 'S'
	while (n && !is_stack_empty()) {
		sform = pop();
		n = expand(sform, n);
		free(sform);
	}
}

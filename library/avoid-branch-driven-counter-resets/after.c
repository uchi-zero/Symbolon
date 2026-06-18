#include<stdio.h>

#define REP(i,n) for(int i = 0; i < (n); i++)
#define STRS(i,str) for(int i = 0; str[i]; i++)

int main(void){
	char ch[120];
	int dum;
	int a = 0, b = 0;
	scanf("%d%s", &dum, ch);
	STRS(i, ch)
		if(ch[i] == '(') a++;
		else{
			a--;
			if(a < b) b = a;
		}
	REP(i, -b) putchar('(');
	printf(ch);
	REP(i, a - b) putchar(')');
	putchar('\n');
}
#include <stdio.h>

int main(){
	int a, b;
	char op;
	
	scanf("%d %c %d", &a, &op, &b);
	printf("%d\n", op=='+'?(a+b):(a-b));
	return 0;
}
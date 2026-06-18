#define scanf_s scanf
#include<stdio.h>
int main(void) {
	int a, b, i;
	for (i = 0; i < 7; i++) {
		scanf_s("%d %d", &a, &b);
		printf("%d\n", a - b);
	}
	
	return(0);
}
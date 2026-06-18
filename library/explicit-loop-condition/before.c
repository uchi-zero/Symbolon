#include<stdio.h>
int main() {
	int a, b, c;
	int  i = 0, l = 0;
	scanf("%d %d %d", &a, &b, &c);
	while (1) {
		l++;
		if (l% 7 == 0)i += b;
		i += a;
		if (i >= c) {
			printf("%d\n", l); break;
		}
	}
}
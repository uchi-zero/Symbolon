#include <stdio.h>
int a[10000];
int x, n;
int main() {
	scanf("%d", &x);
	while (a[x] < 2) {
		a[x]++;
		if (x % 2)x = 3 * x + 1;
		else x /= 2;
		n++;
	}
	printf("%d\n", n - 2);
}
#include <stdio.h>

int main(void) {
	int n, m;
	scanf("%d %d", &n, &m);

	if (n % m == 0) {
		printf("-1\n");
	}
	else
		printf("%d\n", n);
}
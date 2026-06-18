#include <stdio.h>
int main() {
	int N, i;
	scanf("%d", &N);
	for (i = 1; i < 10; i++) {
		if ((N / i < 10) && (N%i == 0)) {
			printf("Yes\n");
			break;
		}
	}
	if (i == 10)
		printf("No\n");
	return 0;
}
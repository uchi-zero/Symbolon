#include<stdio.h>
#define SZ 1001
int k, n, A[SZ][SZ];
int main() {
	int i, j;
	scanf("%d", &k);
	if (k <= 500) {
		printf("%d\n", k);
		for (i = 1; i <= k; i++) {
			for (j = 1; j <= k; j++) {
				printf("%d ", i);
			}
			printf("\n");
		}
	}
	else {
		n = 500;
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				A[i][j] = (i % 2 == 0) ? (i + j) % (n) : (i + j) % n + n;
				if (A[i][j] >= k) {
					A[i][j] -= n;
				}
			}
		}
		printf("%d\n", n);
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				printf("%d ", 1 + A[i][j]);
			}
			printf("\n");
		}
	}
}
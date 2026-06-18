#include<stdio.h>

int main() {

	int k;
	int i, j;
	int n;
	int c[500][500];

	//入力
	scanf("%d", &k);

	if (k == 1) {
		printf("1\n1\n");
		return 0;
	}

	n = (k + 3) / 4 * 2;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			c[i][j] = (i + j) % n + 1;
		}
	}
	for (i = 1; i < n; i += 2) {
		for (j = 0; j < n; j++) {
			if (c[i][j] + n <= k)c[i][j] += n;
		}
	}

	//出力
	printf("%d\n", n);
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%d ", c[i][j]);
		}
		printf("\n");
	}

	return 0;
}
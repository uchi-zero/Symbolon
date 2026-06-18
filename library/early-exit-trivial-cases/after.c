#include <stdio.h>
long long p = 1000000007;
long long ans;
char s[60];
int f[60];
int n, x, y;
int main() {
	scanf("%d", &n);
	scanf(" %s", s);
	scanf(" %s", s);
	if (n == 1) {
		printf("3\n");
		return 0;
	}
	while (x < n - 1) {
		if (s[x] == s[x + 1]) {
			f[y] = 1;
			y++;
			x += 2;
		}
		else {
			f[y] = 0;
			y++;
			x++;
		}
	}
	if (s[n - 1] != s[n - 2]) {
		f[y] = 0;
		y++;
	}
	ans = 3;
	if (f[0])ans *= 2;
	for (int i = 1; i < y; i++) {
		if (f[i - 1] == 0) {
			ans *= 2;
			ans %= p;
		}
		if (f[i] && f[i - 1]) {
			ans *= 3;
			ans %= p;
		}
	}
	printf("%lld\n", ans);
}
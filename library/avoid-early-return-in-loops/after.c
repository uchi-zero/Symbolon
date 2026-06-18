#include <stdio.h>
int n;
int main() {
	scanf("%d", &n);
	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 10; j++) {
			if (n == i*j)n = 0;
		}
	}
	if (n)printf("No\n");
	else printf("Yes\n");
}
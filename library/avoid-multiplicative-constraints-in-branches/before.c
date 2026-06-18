#include <stdio.h>
int d, s, t;
int main() {
	scanf("%d%d%d", &d, &s, &t);
	if (s * t < d)printf("No\n");
	else printf("Yes\n");
}
#include <stdio.h>
int n;
int x, y;
char s[105];
int main() {
	scanf("%d", &n);
	scanf("%s", s);
	for (int i = 0; i < n; i++) {
		if (s[i] == 'B')x++;
		if (s[i] == 'R')y++;
	}
	if (y > x)printf("Yes\n");
	else printf("No\n");
}
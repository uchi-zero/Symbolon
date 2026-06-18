#include<stdio.h>

int main()
{
	int x1, x2, y1, y2;
	scanf("%d%d%d%d", &x1, &y1, &x2, &y2);
	printf("%d %d %d %d\n", x2 - (y2 - y1), y2 + (x2 - x1),
		x1 - (y2 - y1), y1 + (x2 - x1));

	return 0;
}
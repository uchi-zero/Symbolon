#include<stdio.h>
int main()
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	while(~scanf("%d %d %d %d", &x1, &y1, &x2, &y2))
		printf("%d %d %d %d\n", y1-y2+x2, x2-x1+y2, y1-y2+x1, x2-x1+y1);
	return 0;
}
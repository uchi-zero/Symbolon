#include<stdio.h>
int main()
{
	int num;
	scanf("%d", &num);
	int t = 1, s = 1;
	for (int i = 0; i < num; i++)
	{
		int z;
		scanf("%d", &z);
		if (z % 2 == 0)t *= 2;
		s *= 3;
	}
	printf("%d\n", s - t);
}
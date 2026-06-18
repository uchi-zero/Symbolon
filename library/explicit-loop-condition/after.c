#include<stdio.h>
int main(void)
{
	int A = 0, B = 0, C = 0, RESULT = 0, day = 0, coin = 0;

	scanf("%d%d%d", &A, &B, &C);

	while (C > coin)
	{
		RESULT++;
		day++;
		coin += A;

		if (day == 7)
		{
			day = 0;
			coin += B;
		}
	}

	printf("%d\n", RESULT);

	return 0;
}
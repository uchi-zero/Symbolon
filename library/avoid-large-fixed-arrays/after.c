#include <stdio.h>
#include <math.h>




int main()
{
	int i, j, k,n,l;
	scanf("%d", &n);
	printf("2 ");
	l = 1;
	for (i = 3;;i += 2)
	{
		k = 0;
		for (j = 3;j <= sqrt(i);j += 2)
		{
			if (i%j == 0)
			{
				k = 1;
				break;
			}
		}

		if (k == 0&&i%5==1) { printf("%d ", i); ++l;if (l == n) break; }
	}

	printf("\n");

	return 0;
}
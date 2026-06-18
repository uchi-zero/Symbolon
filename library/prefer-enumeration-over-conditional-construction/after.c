#include <stdio.h>
#include <math.h>

int s[11], c[11], d[11], n;

void digit(int N)
{
	int cnt = n;

	while (N)
	{
		d[cnt--] = N % 10;
		N /= 10;
	}
}

int main(void)
{
	int m, i, j;

	scanf("%d %d", &n, &m);
	for (i = 1; i <= m; i++)
		scanf("%d %d", &s[i], &c[i]);

	for (i = n > 1 ? (int)pow(10.0, n - 1.0) : 0; i < (int)pow(10.0, n); i++)
	{
		digit(i);

		for (j = 1; j <= m; j++)
			if (d[s[j]] != c[j])
				break;
		if (j == m + 1)
		{
			printf("%d", i);
			return 0;
		}
	}
	puts("-1");
	return 0;
}
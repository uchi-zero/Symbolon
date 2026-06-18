#include<stdio.h>

int main()
{
	int n;
	scanf("%d", &n);

	for (int i = 0; i * 4 <= n; i++)
	{
		if (!((n - i * 4) % 7))
		{
			printf("Yes\n");
			return 0;
		}
	}
	printf("No\n");

	return 0;
}
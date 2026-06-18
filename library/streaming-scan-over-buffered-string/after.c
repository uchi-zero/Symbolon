#include<stdio.h>
int main(void)
{
	int a = 0, ch;
	while((ch = getchar()) != EOF)
	{
		if (ch == 'C') a = 1;
		else if (ch == 'F' && a)
		{
			printf("Yes");
			return 0;
		}
	}
	printf("No");
	return 0;
}
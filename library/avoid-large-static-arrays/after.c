#include<stdio.h>
int main()
{
	int k;
	scanf("%d",&k);
	if(k==1)
	{
		printf("1\n1\n");
		return 0;
	}
	int n=(((k+3)>>2)<<1);
	printf("%d\n",n);
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			int val=(i+j)%n+1;
			if(i&1)
			{
				val+=n;
				if(val>k)
				{
					val-=n;
				}
			}
			printf("%d",val);
			if(j==n-1)
			{
				putchar('\n');
			}
			else
			{
				putchar(' ');
			}
		}
	}
	return 0;
}
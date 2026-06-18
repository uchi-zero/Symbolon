#include<stdio.h>
int main()
{
	long long a,b,m=1000000;
	scanf("%lld%lld",&a,&b);
	if(b-a>2019)printf("0\n");
	else
	{
		for(long long i=a;i<b;i++)
		{
			for(long long j=i+1;j<=b;j++)
			{
				if((i%2019*j%2019)%2019<m)m=(i%2019*j%2019)%2019;
			}
		}
		printf("%lld\n",m);
	}
}
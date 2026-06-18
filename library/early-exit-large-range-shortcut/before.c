#include<stdio.h>

int main()
{
	unsigned long long a,b,i,j,t=2018,m;
	scanf("%ld%ld",&a,&b);
	for(i=a;i<=b-1;i++)
	{
		for(j=i+1;j<=b;j++)
		{
		m=i*j%2019;
		if(m<t)
		t=m;
		if(t==0)
		break;	
		}
		if(t==0)
		break;
	}
	printf("%ld",t);
 }
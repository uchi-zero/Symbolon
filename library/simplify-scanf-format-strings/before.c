#include<stdio.h>
int main()
{
	long long a,b,c;
	scanf("%lld%lld.%lld",&a,&b,&c);
	b*=100;
	b+=c;
	a*=b;
	a/=100;
	printf("%lld",a);
}
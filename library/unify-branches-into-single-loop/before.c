#include <stdio.h>
#include <math.h>
int main()
{	int n,m,t;
	scanf("%d%d",&n,&m);
	if(m-n<10000000)
	{
		while(m%n) n++;
		printf("%d",m/n);
	}
	else
	{
		t=m/n;
		while(m%t) t--;
		printf("%d",t);
	}
	return 0;
}
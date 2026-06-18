#include<stdio.h>
int main(void)
{
	int n[4],e,f,i,a,b;
	for(i=0;i<4;i++)	{
	scanf("%d",&n[i]);
	}
	scanf("%d %d",&e,&f);
	a=100;
	b=0;
	for(i=0;i<4;i++)	{
		b=b+n[i];
		if(n[i]<a)	{
		a=n[i];
		}
	}
	b=b-a;
	if(e>f)	{
	b=b+e;
	}
	else	{
	b=b+f;
	}
	printf("%d\n",b);
	return 0;
}
#include<stdio.h>

int main()
{
	int s;
	int i=1;

	scanf("%d",&s);
	// a_n=f(n)= ? n/2 : 3n+1
	while(1)
	{
		if(s==4||s==2||s==1) break;
		s=s%2==0?s/2:s*3+1;
		i++;
	}
	printf("%d\n",i+3);
	return 0;
}
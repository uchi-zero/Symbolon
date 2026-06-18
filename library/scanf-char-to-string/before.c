#include<stdio.h>
int main()
{
	char s[5];
	int i,p=0;
	for ( i = 0; i <4; i++)
	{
		scanf("%c",&s[i]);
		if(s[i]=='+')p++;
		if(s[i]=='-')p--;
	}
	printf("%d\n",p);
	return 0;
}
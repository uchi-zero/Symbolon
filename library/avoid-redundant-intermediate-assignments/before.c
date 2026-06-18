#include<stdio.h>
int main(void)
{
	int N,K;
	char s[55];
	scanf("%d %d",&N,&K);
	scanf("%s",&s);
	K-=1;
	s[K]+=32;
	printf("%s\n",s);
	return 0;
}
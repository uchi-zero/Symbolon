#include<stdio.h>
int main()
{
	int  A,B;
	char op;
	scanf("%d %c %d",&A,&op,&B);
	if(op==43)
	printf("%d",A+B); 
	if(op==45)
	printf("%d",A-B);
	return 0;
}
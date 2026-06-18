#include<stdio.h>

int main()
{

	char b, c;

	scanf("%c", &b);

	if (b == 'A')c = 'T';
	if (b == 'T')c = 'A';
	if (b == 'C')c = 'G';
	if (b == 'G')c = 'C';







	printf("%c", c);


	return(0);
}
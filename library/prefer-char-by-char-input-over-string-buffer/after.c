#include<stdio.h>
int main()
{
	char c;
	int ar[27]={0},i;
	while(~scanf("%c", &c))
		ar[c-97]++;
	for(i=0; i<26; i++)
		if(ar[i]%2)
			break;
	if(i==26) printf("Yes\n");
	else printf("No\n");
	return 0;
}
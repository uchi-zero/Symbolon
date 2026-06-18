#include <stdio.h>

int main()
{
	int n, i;
	
	scanf("%d", &n);
	
	for(i=0;1<<(i+1)<=n;i++);
	
	printf("%d", 1<<i);
	
	return 0;
}
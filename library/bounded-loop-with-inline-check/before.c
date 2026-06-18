#include <stdio.h>


int main()
{		
	long long n;
	int i;
	char s[111];
	
	scanf("%s%lld", s, &n);
	for(i = 0; s[i] && s[i] == 49; i++);
	if(n > i) printf("%c\n", s[i]);
	else puts("1");
				
	return 0;
}
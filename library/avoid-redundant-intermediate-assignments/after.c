#include <stdio.h>
int main (){
	int n,k;
	char s[60] ;
	
	scanf("%d %d",&n ,&k);
	scanf("%s",&s);
	s[k-1] += 32;
	printf("%s",s);
	
	
	return 0;
}
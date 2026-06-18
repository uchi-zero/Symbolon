#include <stdio.h>
int main(){
	char s[105];scanf("%s",s);
	for(register int i=0;s[i];i++) if(s[i]=='C') for(register int j=i+1;s[j];j++) if(s[j]=='F') return puts("Yes"),0;
	puts("No");
	return 0;
}
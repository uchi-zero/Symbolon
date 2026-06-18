#include<stdio.h>
int main(void)
{
	int i,a,b,cnt;
	char s[1000];
	scanf("%d%d%s",&a,&b,s);
	cnt=0;
	for(i=0;i<a+b+1;i++){
		if(s[i]!='-') cnt++;
	}
	if(s[a]=='-'&&a+b==cnt) printf("Yes\n");
	else printf("No\n");
	return 0;
}
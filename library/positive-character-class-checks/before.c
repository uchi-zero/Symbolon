#include<stdio.h>
int main(void)
{
	int n,m,flg=1,i;
	char s[11];
	scanf("%d %d",&n,&m);
	scanf("%s",s);
	for(i=0;i<n;i++){
		if(s[i]=='-') flg=0;
	}
	for(i=n+1;i<n+m+1;i++){
		if(s[i]=='-') flg=0;
	}
	if(s[n]!='-') flg=0;
	if(flg==1) printf("Yes\n");
	else printf("No\n");
	return 0;
}
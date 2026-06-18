#include <stdio.h>
int main()
{
	int n,m,k,flag=0,i,j;
	scanf("%d %d %d",&n,&m,&k);
	for(i=0;i<=n;i++)
	for(j=0;j<m+1;j++)
	{
		if(i*(m-j)+j*(n-i)==k)
		{
			flag=1;break;
		}
	}
	if(flag)printf("Yes\n");
	else printf("No\n");
}
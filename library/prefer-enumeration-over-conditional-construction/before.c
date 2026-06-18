#include<stdio.h>
int main()
{
	int n,m,x,y;
	int a[5]={0},b[5]={0};
	scanf("%d%d",&n,&m);
	int flag=1;
	while(m--)
	{
		scanf("%d%d",&x,&y);
		if(b[x]==0||a[x]==y) {a[x]=y;b[x]=1;}
		else flag=0;
	}

	if(flag){
		if(n==1)	printf("%d\n",a[1]);
		else{
			if(a[1]==0&&b[1]==0)	a[1]=1;
			if(a[1]==0&&b[1]==1)
			{
				printf("-1\n");
				return 0;
			}
			for(int i=1;i<=n;i++)
			printf("%d",a[i]);
			printf("\n");
			}
		}
	else printf("-1\n");
	
	return 0;
 }
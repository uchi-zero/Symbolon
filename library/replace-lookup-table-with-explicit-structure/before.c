#include<stdio.h>
int main()
{
	int c,n,s,t,b,i,j,k,x[9][4]={
		{1,3,0,0},
		{0,2,4,1},
		{1,5,2,2},
		{0,4,6,3},
		{1,3,5,7},
		{2,4,8,5},
		{3,7,6,6},
		{4,6,8,7},
		{5,7,8,8}
	};
	char u,v,w;
	while(scanf("%d",&n),n)
	{
		int a[16][9]={0};
		scanf(" %c %c %c",&u,&v,&w);
		s=u-'A',t=v-'A',b=w-'A';
		c=a[0][s]=1;
		for(i=0;i<n;++i,c*=4)for(j=0;j<9;++j)for(k=0;k<4;++k)
			if(x[j][k]!=b)a[i+1][x[j][k]]+=a[i][j];
			else a[i+1][j]+=a[i][j];
		printf("%.8f\n",a[n][t]/(double)c);
	}
	return 0;
}
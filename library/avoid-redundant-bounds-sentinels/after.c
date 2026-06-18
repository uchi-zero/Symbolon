#include<stdio.h>
int main()
{
	const int N = 100;
	char a[N][N];

	int n,d,caseCount=0,i,j,x,y,dd[]={-1,0,1,0},angle,c;
	scanf("%d",&d);
	while(d--)
	{
		scanf("%d",&n);
		for(i=0;i<n;++i)for(j=0;j<n;++j)a[i][j]=' ';

		x=0;y=n-1;
		angle=0;
		for(c=0;c<3;++c)
		{
			for(;x>=0&&y>=0&&x<n&&y<n; x+=dd[(angle+1)%4],y+=dd[angle])
			{
				a[y][x]='#';
			}
			x-=dd[(angle+1)%4];
			y-=dd[angle];
			angle=(angle+1)%4;;
		}
		if(n>2)
		{
			for(;n;)
			{
				c=0;
				for(; a[y+2*dd[angle]][x+2*dd[(angle+1)%4]]!='#'; x+=dd[(angle+1)%4],y+=dd[angle])
					a[y][x]='#',c++;
				if(c<2) break;
				angle=(angle+1)%4;;
			}
			a[y][x]='#';
		}
		else if(n==2)
			a[n-1][n-1]=' ';

		printf("%s",(caseCount++?"\n":""));
		for(i=0;i<n;++i)
		{
			for(j=0;j<n;++j)
				putchar(a[i][j]);
			putchar('\n');
		}
	}
	return 0;
}
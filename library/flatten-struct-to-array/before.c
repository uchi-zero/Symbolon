#include<stdio.h>
int main()
{
	int w,h,i,j,N=100000;
	while(scanf("%d%d",&w,&h),w)
	{
		struct S{int e1,e2,n1,n2;}a[102][102],s;
		s.e1=s.e2=s.n1=s.n2=0;
		for(i=0;i<h;++i)for(j=0;j<w;++j)a[i][j]=s;
		a[0][0].n2=a[0][0].e2=1;
		for(i=0;i<h;++i)for(j=0;j<w;++j)
		{
			s=a[i][j];
			a[i][j+1].e1=(a[i][j+1].e1+s.e1+s.e2)%N;
			a[i][j+1].e2=s.n1;
			a[i+1][j].n1=(a[i+1][j].n1+s.n1+s.n2)%N;
			a[i+1][j].n2=s.e1;
		}
		s=a[h-1][w-1];
		printf("%d\n",(s.e1+s.e2+s.n1+s.n2)%N);
	}
	return 0;
}
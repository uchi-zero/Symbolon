#include<stdio.h>

int main()
{
	int l,r;
	long long i,j;
	scanf("%d%d",&l,&r);
	if (r-l>2019) printf("0\n");
	else {
		int mm=2019;
		for (i=l;i<=r;i++)
		for (j=i+1;j<=r;j++) {
			int tt=(i*j)%2019;
			if (tt<mm) mm=tt;
		}
		printf("%d\n",mm);
	}
	return 0;
}
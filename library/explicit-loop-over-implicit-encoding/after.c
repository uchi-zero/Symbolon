#include <stdio.h>
int math[]={-1,1};
int main()
{
	int a[4];register int i,j,k;
	for(i=0; i<4; ++i)
	{
		char c=getchar();
		a[i]=c-'0';
	}
	for (i=0; i<2; ++i)
		for (j=0; j<2; ++j)
			for (k=0; k<2; ++k)
				if(a[0]+math[i]*a[1]+math[j]*a[2]+math[k]*a[3]==7)
				{
					printf("%d",a[0]);
					printf("%c",i==0?'-':'+');
					printf("%d",a[1]);
					printf("%c",j==0?'-':'+');
					printf("%d",a[2]);
					printf("%c",k==0?'-':'+');
					printf("%d",a[3]);
					printf("=7\n");
					return 0;
				}
}
#include <stdio.h>
#include <math.h>

int main(void){
	double n,i,j,k,ans,ap;
	scanf("%lf",&n);
	ans = 4/n;
	for (i=1;i<5000;i++)
	{
		for (j=1;j<5000;j++)
		{
			ap = ans - 1/i - 1/j;
			if (ap >0)
			{
				ap = 1.0/ap;
				ap = roundl(ap * 10000000) / 10000000;
				if ((int)(ap) == ap) 
				{
					printf("%d %d %d",(int)(i),(int)(j),(int)(ap));
					return 0;
				}
			}
		}
	}
	return 0;
}
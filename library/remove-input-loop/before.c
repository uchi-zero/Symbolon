#include<stdio.h>
#include<math.h>
int main()
{
	int n,a;
	float b;
	while(~scanf("%d", &n))
	{
		a=ceil(n/1.08);
		if(floor(a*1.08)==n)
			printf("%d\n", a);
		else 
			printf(":(\n");
	}
	return 0;
}
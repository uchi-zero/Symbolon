#include <stdio.h>
int f[60005];
int main()
{
	for (int x = 1;x<=100;x++)
		for (int y = 1;y<= 100;y++)
			for (int z = 1;z <= 100;z++)
			{
				int ans = x*x+y*y+z*z+x*y+y*z+x*z;
				f[ans]++;
			}
	int n;
	scanf("%d",&n);
	for (int i = 1;i<= n;i++)
		printf("%d\n",f[i]);
	return 0;
}
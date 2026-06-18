#include<stdio.h>
int main()
{
	long long  x,y,z;
	scanf("%lld%lld%lld",&x,&y,&z);
	int lose=0;long long anc=1;
	if(y-z<=0&&y>=x)
		lose=1;
	else
		if(y-z>0)
		{
			
			anc=(x-y)/(y-z);
			if(anc*(y-z)+y>=x)  
				anc=anc*2+1;
			else
			{
				anc=(anc+1)*2+1;
			}
			lose=1;
		}
		else
			lose=0;
	if(lose)
		printf("%lld\n",anc);
	else
		printf("-1\n");
		
}
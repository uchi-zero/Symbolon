#include<stdio.h>
#include<math.h>
int main()
{
	int n,a,i;
	long long int sum,sub;
	sum=1;
	sub=1;
	scanf("%d",&n);
	for (i=1;i<=n;i++)
	{
		sum=sum*3;
		scanf("%d",&a);
		if (a%2==0)
			sub=sub*2;
	}
	printf("%lld\n",sum-sub);
	return 0;

	


}
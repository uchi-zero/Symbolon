#include<stdio.h>
#define ll long long
const ll mod=1e9+7;
ll dp[2005]={0},n;
int main()
{
	scanf("%lld",&n);
	dp[0]=1;
	dp[1]=0;
	dp[2]=0;
	for(ll i=3;i<=n;i++)
	{
		for(ll j=3;i-j>=0;j++)
		{
			dp[i]+=dp[i-j];
			dp[i]%=mod;
		}
//	printf("%lld\n",dp[i]);
		
	}
	printf("%lld\n",dp[n]);
 }
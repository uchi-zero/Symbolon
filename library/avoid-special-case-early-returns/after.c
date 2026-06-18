#include <stdio.h>
#define MOD 998244353
long long int DP[3010][3010];
int main()
{
	int a,b,c,d;
	scanf("%d%d%d%d",&a,&b,&c,&d);
	
	long long int C = 1;
	for(int j=c;j>=a;j--)
	{
		DP[d][j] = C;
		C*=d;
		C%=MOD;
	}
	
	for(int i=d-1;i>=b;i--)
	{
		C = 0;
		for(int j=c;j>=a;j--)
		{
			DP[i][j] = (C + j*DP[i+1][j])%MOD;
			
			//C *= i;
			C += DP[i+1][j];
			C *= i;
			C %= MOD;
			//printf("%d %d %lld!!\n",i,j,DP[i][j]);
		}
	}
	
	printf("%lld",DP[b][a]);
}
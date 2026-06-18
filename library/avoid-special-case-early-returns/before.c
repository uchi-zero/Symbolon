#include<stdio.h>
#pragma GCC optimize ("O2")
#define rep(i,N) for(int i=0;i<(int)N;i++)
const int MOD=998244353;
int Pow(int x,int y){int z=1;x%=MOD;while(y){if(y&1){z=(1ll*z*x)%MOD;}x=(1ll*x*x)%MOD;y>>=1;}return z;}
int main(void)
{
  int A,B,C,D,ans=0,sum,dp[3000]={0};
  if(!scanf("%d %d %d %d",&A,&B,&C,&D))return 0;
  if(A==C){return printf("%d\n",Pow(A,D-B)),0;}
  else if(B==D){return printf("%d\n",Pow(B,C-A)),0;}
  dp[A-1]=1;
  rep(i,D-B)
  {
    sum=0;
    rep(j,C)
    {
      sum=(1ll*sum*(B+i)%MOD+dp[j])%MOD;
      dp[j]=(1ll*j*dp[j]+sum)%MOD;
    }
  }
  rep(i,C){ans=(ans+1ll*dp[i]*Pow(D,C-i-1))%MOD;}
  return printf("%d\n",ans),0;
}
#include<stdio.h>
typedef long long ll;
const int mod=998244353;
int mul(int a,int b){return a*(ll)b%mod;}
int ad(int a,int b){return(a+b)%mod;}
int pow(int a,int b){
	int s=1;
	while(b){
		if(b&1)s=mul(s,a);
		a=mul(a,a);
		b>>=1;
	}
	return s;
}
int fac[300010],rfac[300010];
int C(int n,int k){return mul(fac[n],mul(rfac[k],rfac[n-k]));}
int main(){
	int n,i,ans;
	ll a,b,k;
	scanf("%d%lld%lld%lld",&n,&a,&b,&k);
	fac[0]=1;
	for(i=1;i<=n;i++)fac[i]=mul(fac[i-1],i);
	rfac[n]=pow(fac[n],mod-2);
	for(i=n;i>0;i--)rfac[i-1]=mul(rfac[i],i);
	ans=0;
	for(i=0;i<=n&&i*a<=k;i++){
		if((k-i*a)%b==0&&(k-i*a)/b<=(ll)n)ans=ad(ans,mul(C(n,i),C(n,(k-i*a)/b)));
	}
	printf("%d",ans);
}
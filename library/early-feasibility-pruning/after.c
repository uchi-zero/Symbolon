#include <stdio.h>
const int M=998244353;
long long f[300010]={1};
int gcd(int x,int y){return y?gcd(y,x%y):x;}
long long inv(long long x){
	long long t=1;
	for(int i=0;i<30;++i){
		if((M-2)&(1<<i))t=t*x%M;
		x=x*x%M;
	}
	return t;
}
long long c(int n,int r){
	return f[n]*inv(f[r])%M*inv(f[n-r])%M;
}
int main(){
	int i,n,x,y,t;
	long long a,b,k,ans=0;
	scanf("%d%lld%lld%lld",&n,&a,&b,&k);
	t=gcd(a,b);
	if(k%t)return !puts("0");
	for(i=1;i<=n;++i)f[i]=f[i-1]*i%M;
	a/=t,b/=t,k/=t;
	for(x=0;x<=n;++x){
		if(a*x>k)break;
		if((k-a*x)%b)continue;
		y=(k-a*x)/b;
		if(y<0||y>n)continue;
		ans=(ans+c(n,x)*c(n,y))%M;
	}
	printf("%lld",ans);
	return 0;
}
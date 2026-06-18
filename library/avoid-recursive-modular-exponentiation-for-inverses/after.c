#include <stdio.h>

const long long mod = 1000000007;
long long inv[200200],fact[200200],ifact[200200];

long long comb(int n, int k)
{
	return fact[n] * ifact[k] % mod * ifact[n-k] % mod;
}

int main()
{
	inv[1] = fact[0] = fact[1] = ifact[0] = ifact[1] = 1;
	for (int i=2;i<200200;i++){
		inv[i] = (mod - mod / i) * inv[mod % i] % mod;
		fact[i] = fact[i-1] * i % mod;
		ifact[i] = ifact[i-1] * inv[i] % mod;
	}

	int h,w,a,b; scanf ("%d %d %d %d",&h,&w,&a,&b);

	long long ans = comb(h+w-2,h-1);

	for (int i=0;i<b;i++){
		long long u = comb(h-a-1+i,i);
		long long v = comb(a-1+w-1-i,a-1);
		ans = (ans - u * v % mod + mod) % mod;
	}
	printf ("%lld\n",ans);

	return 0;
}
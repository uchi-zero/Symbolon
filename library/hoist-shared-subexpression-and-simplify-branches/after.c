#include <stdio.h>
typedef long long LL;

int main()
{	 
	LL x, k, d, y, z;
	
	scanf("%lld%lld%lld", &x, &k, &d);	
	if(x < 0) x = -x;
	y = x / d;
	if(y >= k) z = x - d * k;
	else{
		z = x % d;
		if((k - y) & 1) z = d - z;
	}
	printf("%lld\n", z);
		
	return 0;
}
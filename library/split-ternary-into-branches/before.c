#include <stdio.h>

int main()
{
long long a,b; scanf ("%lld %lld",&a,&b); printf("%lld\n",a==1||b==1?1:(a*b+1)/2);
}
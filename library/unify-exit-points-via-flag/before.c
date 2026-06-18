#include <stdio.h>
#include <stdlib.h>
typedef long long ll;
ll a,b,c,k;
int main()
{
    scanf("%lld %lld %lld",&a,&b,&c);
    if(b>=a)
    {
        printf("1\n");
        exit(0);
    }
    if(c>=b)
    {
        printf("-1\n");
        exit(0);
    }
    if((a-b)%(b-c)>=1)
		k=1;
    printf("%lld\n",((a-b)/(b-c)+k)*2+1);
}
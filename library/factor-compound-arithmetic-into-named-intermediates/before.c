#include <stdio.h>
long long s;
int main(void){
    scanf("%lld",&s);
    printf("%lld %lld %lld %lld %lld %lld\n",0,0,1,1000000000,(s-1)/1000000000+1,((s-1)/1000000000+1)*1000000000-s);
}
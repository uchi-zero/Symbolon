#include<stdio.h>
long long f(long long x,long long y)
{
    if(x==0) return -y;
    else return (y/x)*x*2 + f(y%x,x);
}
int main()
{
    long long n,x;
    scanf("%lld%lld",&n,&x);
    printf("%lld",f(x,n-x)+n);
}
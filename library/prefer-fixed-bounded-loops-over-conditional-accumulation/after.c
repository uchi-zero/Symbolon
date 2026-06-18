#include <stdio.h>

int gcd(int a, int b)
{
    if(b==0) return a;
    return gcd(b, a%b);
}

int lcm(int a, int b)
{
    return (a/gcd(a,b))*b;
}

int a[20];

int main()
{
    int n, l, i;
    while(scanf("%d", &n)==1){
        l = 0;
        scanf("%d", &a[0]);
        scanf("%d", &a[1]);
        l = lcm(a[0], a[1]);
        for(i=2; i<n; i++){
            scanf("%d", &a[i]);
            l = lcm(l, a[i]);
        }
        printf("%d\n", l);
    }
    return 0;
}
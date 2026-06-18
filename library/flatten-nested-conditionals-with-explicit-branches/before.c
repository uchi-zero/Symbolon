#include <stdio.h>

int min(int a, int b) { return (a > b) ? b : a; }

int main()
{
    int t; scanf("%d", &t);
    int c[3];
    int a[5];

    while(t--)
    {
        for(int i = 0; i < 3; i++)
            scanf("%d", &c[i]);
        for(int i = 0; i < 5; i++)
            scanf("%d", &a[i]);
        if(a[0] > c[0])
        {
            puts("NO");
            continue;
        }
        c[0] -= a[0];
        a[3] -= min(c[0], a[3]);
        if(a[1] > c[1])
        {
            puts("NO");
            continue;
        }
        c[1] -= a[1];
        a[4] -= min(c[1], a[4]);
        if(c[2] >= a[2]+a[3]+a[4])
            puts("YES");
        else
            puts("NO");
    }
}
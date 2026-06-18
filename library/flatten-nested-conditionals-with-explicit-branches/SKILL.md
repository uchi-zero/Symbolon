---
name: flatten-nested-conditionals-with-explicit-branches
description: Use when code uses early-exit continue/break patterns combined with min() helpers and in-place arithmetic updates that implicitly couple constraints across iterations
---

# flatten-nested-conditionals-with-explicit-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses early-exit continue/break patterns combined with min() helpers and in-place arithmetic updates that implicitly couple constraints across iterations

## Transformation

Replace early-exit guards and min()-based saturating subtractions with a flat sequence of explicit if/else branches that directly compare linear combinations of the original input variables

## Why it helps

KLEE handles linear arithmetic constraints over fresh symbolic inputs much better than constraints over variables mutated by min() (which introduces ITE expressions) and loop-carried state; explicit branches expose each feasible path as a distinct linear constraint system, letting the solver enumerate and cover more paths quickly

## Example

Before:

```c
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
```

After:

```c
#include<stdio.h>
void total();
int main()
{
    int t,i;
    scanf("%d",&t);
    for(i=1;i<=t;i++)
    {
        total();
    }
}
void total()
{
   long long int c1,c2,c3,a1,a2,a3,a4,a5;
   scanf("%lld %lld %lld",&c1,&c2,&c3);
   scanf("%lld %lld %lld ",&a1,&a2,&a3);
   scanf("%lld  %lld",&a4,&a5);
   if(c1>=a1&&c2>=a2&&c3>=a3)
   {
       if(a4<=(c1-a1)+(c3-a3))
        {
            if(a4-(c1-a1)>=0)
        {

            if(a5<=(c2-a2)+(c3-a3)-(a4-(c1-a1)))
                printf("YES\n");
             else printf("NO\n");

        }
        if(a4-(c1-a1)<0)
            if(a5<=(c2-a2)+(c3-a3))
                printf("YES\n");
            else printf("NO\n");
        }
        else printf("NO\n");
       //printf("c1=%lld,c3=%lld,s1=%lld,s2=%lld\n",c1,c3,s1,s2);


        }
        else
            printf("NO\n");

}
```

## Evidence

- Average coverage delta: 0.5082
- Source problems: train_Codeforces_1468_N
- Rule id: `0d060b92`

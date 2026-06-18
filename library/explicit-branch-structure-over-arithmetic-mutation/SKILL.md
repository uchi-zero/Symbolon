---
name: explicit-branch-structure-over-arithmetic-mutation
description: Use when the code mutates input variables in-place via chained arithmetic (e.g., subtracting, then conditionally zeroing, then subtracting again) before a final aggregate predicate, collapsing distinct logical cases into a single path
---

# explicit-branch-structure-over-arithmetic-mutation

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code mutates input variables in-place via chained arithmetic (e.g., subtracting, then conditionally zeroing, then subtracting again) before a final aggregate predicate, collapsing distinct logical cases into a single path

## Transformation

Refactor into explicit nested if/else branches that directly encode each logical case over the original unmodified inputs, so each feasible scenario corresponds to a distinct control-flow path with its own printf

## Why it helps

KLEE forks on branch conditions; replacing arithmetic-encoded case analysis with explicit conditionals exposes more discrete paths and produces simpler per-path constraints (linear inequalities over inputs) that the solver handles faster, yielding test inputs that cover more behaviors

## Example

Before:

```c
#include<stdio.h>
int main()
{
    int a,b[3],c[5],i,j,k,t;
    scanf("%d",&a);
    for(i=1;i<=a;i++)
    {
         for(j=0;j<=2;j++)
               scanf("%d",&b[j]);

           for(j=0;j<=4;j++)
                scanf("%d",&c[j]);

           for(j=0;j<=2;j++)
             b[j]=b[j]-c[j];



           if(c[3]>b[0])
              c[3]=c[3]-b[0];
           else if(c[3]<=b[0])
              c[3]=0;

               if(c[4]>b[1])
                  c[4]=c[4]-b[1];

              else if(c[4]<=b[1])
                   c[4]=0;


              b[2]=b[2]-c[3]-c[4];
            if((b[0]>=0)&&(b[1]>=0)&&(b[2]>=0))
                   printf("YES\n");
            else
                printf("NO\n");
        }
       return 0;
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

- Average coverage delta: 0.3242
- Source problems: train_Codeforces_1468_N
- Rule id: `8b5615fc`

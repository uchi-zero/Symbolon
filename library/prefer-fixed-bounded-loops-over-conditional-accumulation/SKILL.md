---
name: prefer-fixed-bounded-loops-over-conditional-accumulation
description: Use when the code uses a single loop with an index-based conditional (e.g., if i==0 initialize, else accumulate) and branches inside the loop on operand ordering before invoking a helper
---

# prefer-fixed-bounded-loops-over-conditional-accumulation

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code uses a single loop with an index-based conditional (e.g., if i==0 initialize, else accumulate) and branches inside the loop on operand ordering before invoking a helper

## Transformation

Hoist the first one or two iterations out of the loop to perform explicit initialization, drop redundant ordering branches before commutative helper calls, and use a simple bounded loop over a fixed-size array for the remaining accumulation

## Why it helps

Removing per-iteration initialization branches and symmetric ordering branches collapses path explosion in KLEE; fixed bounded loops over small arrays produce a predictable, shallow path tree so symbolic execution covers more states within the same budget

## Example

Before:

```c
#include <stdio.h>

long long calc(long long x,long long y){
	if(y == 0){
		return x;
	}else{
		return calc(y,x%y);
	}
}

int main(){
	int n;
	long long tmp,number,r_1,r_2;

	scanf("%d",&n);

	for(int i=0; i < n; i++){
		scanf("%lld",&tmp);
		if(i == 0){
			number = tmp;
		}else{
			if(number >= tmp){
				r_1 = calc(number,tmp);
				number = (number*tmp)/r_1;
			}else{
				r_1 = calc(tmp,number);
				number = (number*tmp)/r_1;
			}
		}
	}

	printf("%lld\n",number);
}
```

After:

```c
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
```

## Evidence

- Average coverage delta: 0.2238
- Source problems: train_AIZU_p02469
- Rule id: `9e71e21e`

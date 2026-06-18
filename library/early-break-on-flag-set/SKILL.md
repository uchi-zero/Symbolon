---
name: early-break-on-flag-set
description: Use when a nested loop continues exhaustively after a success condition sets a flag, with no early exit
---

# early-break-on-flag-set

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a nested loop continues exhaustively after a success condition sets a flag, with no early exit

## Transformation

Add a break statement immediately after setting the success flag so the loop terminates as soon as the satisfying condition is found

## Why it helps

KLEE forks a path on every iteration's branch condition; without an early break, the symbolic executor must explore all remaining (i,j) combinations even after success, multiplying paths and constraint-solving work. Breaking early prunes redundant subtrees, letting KLEE reach diverse program states and produce higher-coverage test inputs within the same budget

## Example

Before:

```c
#include <stdio.h>

int main () {
    int i,j,m = 0;
    int N,M,K;
    scanf("%d %d %d",&N,&M,&K);
    for (i=0;i<=N;i++){
        for (j=0;j<=M;j++){
            if (j*(N - i)+i*(M - j) == K){m=1;}
        }
    }
    if (m==1){printf("Yes\n");}
    else{printf("No\n");}
    return 0;
}
```

After:

```c
#include <stdio.h>
int main()
{
	int n,m,k,flag=0,i,j;
	scanf("%d %d %d",&n,&m,&k);
	for(i=0;i<=n;i++)
	for(j=0;j<m+1;j++)
	{
		if(i*(m-j)+j*(n-i)==k)
		{
			flag=1;break;
		}
	}
	if(flag)printf("Yes\n");
	else printf("No\n");
}
```

## Evidence

- Average coverage delta: 0.2143
- Source problems: train_AtCoder_p03592
- Rule id: `b4f11152`

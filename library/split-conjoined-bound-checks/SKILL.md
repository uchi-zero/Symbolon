---
name: split-conjoined-bound-checks
description: Use when a single boolean condition combines multiple independent range/bound checks using two-sided inequalities (e.g., r<=x && x<=W-r) joined with &&
---

# split-conjoined-bound-checks

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a single boolean condition combines multiple independent range/bound checks using two-sided inequalities (e.g., r<=x && x<=W-r) joined with &&

## Transformation

Rewrite each two-sided bound check as a single-sided inequality with the variable isolated on one side (e.g., 0<=x-r && x+r<=W), keeping each conjunct as a simple linear comparison against a constant or single sum

## Why it helps

KLEE's constraint solver (STP/Z3) handles simpler linear inequalities with isolated variables more efficiently, and the decomposed form yields cleaner path constraints that generalize better when replayed, increasing branch coverage on related solutions

## Example

Before:

```c
#include <stdio.h>
int main(){
  int W,H,x,y,r;
  scanf("%d%d%d%d%d",&W,&H,&x,&y,&r);
  if((r<=x)&&(x<=W-r)&&(r<=y)&&(y<=H-r)){
    printf("Yes\n");
  }else{
    printf("No\n");
  }
}
```

After:

```c
#include<stdio.h>
int main(void)
{
	int w,h,x,y,r;
	scanf("%d %d %d %d %d",&w,&h,&x,&y,&r);
	if(0<=x-r&&0<=y-r&&x+r<=w&&y+r<=h){
					printf("Yes\n");
		}
	else{
			printf("No\n");
		}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1742
- Source problems: train_AIZU_p02394
- Rule id: `20864ef9`

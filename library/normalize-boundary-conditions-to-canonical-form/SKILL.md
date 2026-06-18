---
name: normalize-boundary-conditions-to-canonical-form
description: Use when boundary checks are written as a disjunction of violation conditions (e.g., x+r>W || x-r<0) using strict inequalities and mixed operand orderings
---

# normalize-boundary-conditions-to-canonical-form

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When boundary checks are written as a disjunction of violation conditions (e.g., x+r>W || x-r<0) using strict inequalities and mixed operand orderings

## Transformation

Rewrite the check as a conjunction of the valid-range conditions using consistent canonical form (e.g., 0<=x-r && x+r<=W), placing constants on the left and variables on the right with non-strict inequalities

## Why it helps

KLEE's constraint solver handles canonical affine inequalities (var OP const) more efficiently, and conjunctions of valid-range predicates produce cleaner path constraints than negated disjunctions of violations, enabling better caching and faster feasibility checks across paths

## Example

Before:

```c
#include<stdio.h>
int main(){
int W,H,x,y,r;
scanf("%d %d %d %d %d",&W,&H,&x,&y,&r);
if(x+r>W||x-r<0||y+r>H||y-r<0)
printf("No\n");
else
printf("Yes\n");
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

- Average coverage delta: 0.1763
- Source problems: train_AIZU_p02394
- Rule id: `85ceca3f`

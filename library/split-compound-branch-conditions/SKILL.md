---
name: split-compound-branch-conditions
description: Use when a single conditional chain conflates multiple distinct geometric/logical cases using compound expressions (e.g., comparing against (ar+br)^2 and (ar-br)^2 in one if/else-if ladder), forcing KLEE to solve nonlinear constraints over fewer, more complex path predicates
---

# split-compound-branch-conditions

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a single conditional chain conflates multiple distinct geometric/logical cases using compound expressions (e.g., comparing against (ar+br)^2 and (ar-br)^2 in one if/else-if ladder), forcing KLEE to solve nonlinear constraints over fewer, more complex path predicates

## Transformation

Decompose the unified comparison ladder into separate explicit branches for each distinct case (one circle inside another, tangent internally, tangent externally, intersecting, disjoint), each with its own simple equality/inequality predicate on directly-named quantities like di+r1<r2, di==r1+r2, etc.

## Why it helps

Splitting compound branches into many simpler conditions creates more independent path predicates, each with simpler constraints that KLEE's solver handles faster, leading to broader path coverage and test inputs that exercise more distinct equivalence classes when replayed.

## Example

Before:

```c
#include <stdio.h>
int ax, ay, ar, bx, by, br;
int main() {
	scanf("%d%d%d%d%d%d", &ax, &ay, &ar, &bx, &by, &br);
	int norm = (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
	if(norm > (ar + br) * (ar + br)) puts("4");
	else if(norm == (ar + br) * (ar + br)) puts("3");
	else if(norm > (ar - br) * (ar - br)) puts("2");
	else if(norm == (ar - br) * (ar - br)) puts("1");
	else puts("0");
}
```

After:

```c
#include<stdio.h>
#include<math.h>

int main(){
		double a,b,r1,d,e,r2;
		scanf("%lf%lf%lf%lf%lf%lf",&a,&b,&r1,&d,&e,&r2);

		double di=sqrt((a-d)*(a-d)+(b-e)*(b-e));

		if(di+r1<r2)printf("0\n");
		else if(di+r2<r1)printf("0\n");
		else if(di==r2+r1)printf("3\n");
		else if(di>r2+r1)printf("4\n");
    else if(di+r2==r1)printf("1\n");
    else if(di+r1==r2)printf("1\n");
    else printf("2\n");
}
```

## Evidence

- Average coverage delta: 0.2222
- Source problems: train_AIZU_p02305
- Rule id: `e4f1bc51`

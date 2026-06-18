---
name: split-branches-by-outer-case
description: Use when a single chain of conditionals encodes multiple symbolic regimes (e.g., relies on a squared-norm comparison that collapses cases like c1r<c2r vs c1r>=c2r into one path) producing few distinct branch points for KLEE to fork on
---

# split-branches-by-outer-case

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a single chain of conditionals encodes multiple symbolic regimes (e.g., relies on a squared-norm comparison that collapses cases like c1r<c2r vs c1r>=c2r into one path) producing few distinct branch points for KLEE to fork on

## Transformation

Introduce an explicit outer case split on the distinguishing variable relationship and duplicate the inner conditional chain inside each branch, so each comparison becomes its own forkable branch with simpler per-branch constraints

## Why it helps

More syntactic branches give KLEE more fork points and finer-grained path conditions, and splitting compound algebraic predicates into separate linear comparisons yields simpler constraints that the solver can satisfy independently, increasing achievable coverage

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
#include <stdio.h>
#include <math.h>

int main(int argc, const char * argv[]) {

    int c1x, c1y, c1r, c2x, c2y, c2r;
    scanf("%d %d %d", &c1x, &c1y, &c1r);
    scanf("%d %d %d", &c2x, &c2y, &c2r);
    
    if(c1r<c2r){
        if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) > c1r + c2r) printf("4\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c1r + c2r) printf("3\n");
        else if((c2r-c1r<sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2))) && (sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c1r + c2r)) printf("2\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c2r-c1r) printf("1\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c2r-c1r) printf("0\n");
        
        
    }
    
    else{
        if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) > c1r + c2r) printf("4\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c1r + c2r) printf("3\n");
        else if(((c1r-c2r<sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2))) && (sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c1r + c2r))) printf("2\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c1r-c2r) printf("1\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c1r-c2r) printf("0\n");
    }
}
```

## Evidence

- Average coverage delta: 0.2222
- Source problems: train_AIZU_p02305
- Rule id: `ecc69ed8`

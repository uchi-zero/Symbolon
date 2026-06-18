---
name: loop-to-explicit-cases
description: Use when nested loops with arithmetic constraints are used to check if an equation has integer solutions
---

# loop-to-explicit-cases

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops with arithmetic constraints are used to check if an equation has integer solutions

## Transformation

Replace loops that search for solutions with explicit enumeration of non-solution cases or direct mathematical analysis

## Why it helps

KLEE struggles with nested loops containing arithmetic constraints, while explicit conditionals create independent paths that are easier to explore and generate diverse test inputs for

## Example

Before:

```c
#include <stdio.h>

int main(){
  int N;
  scanf("%d",&N);
  for(int i=0;i<100;i++){
    for(int j=0;j<100;j++){
      if(4*i+7*j==N) {printf("Yes"); return 0;}
    }
  }
  printf("No");
}
```

After:

```c
#include <stdio.h>
int main(void)
{
	int n;
	scanf("%d",&n);
	if(n==17 || n==13 || n==10 || n==9 || n==6 || n==5 || n==3 || n==2 || n==1){
		printf("No\n");
	}else{
		printf("Yes\n");
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1828
- Source problems: train_AtCoder_p03285
- Rule id: `cb17537d`

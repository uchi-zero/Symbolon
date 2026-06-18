---
name: bitmask-instead-of-array-flags
description: Use when code uses an auxiliary array indexed by small values to mark set membership and tests membership inside a loop with early-exit branching
---

# bitmask-instead-of-array-flags

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses an auxiliary array indexed by small values to mark set membership and tests membership inside a loop with early-exit branching

## Transformation

Replace the array-of-flags plus break-on-match loop with a bitmask: encode the set as bits in an integer, compute a bitmask of the queried digits/elements, and test membership with a single bitwise AND

## Why it helps

KLEE forks on every array-index load and on the conditional break inside the digit loop, multiplying paths per input; collapsing membership testing into a branch-free bitwise AND eliminates per-iteration forks and yields simpler, more solver-friendly constraints

## Example

Before:

```c
#include <stdio.h>
int D[10];

int main(){
  int N,K,i,h,flag,temp;
  scanf("%d %d",&N,&K);
  for(i=0;i<K;i++){
    scanf("%d",&h);
    D[h]=1;
  }
  
  do{
    flag=0;
    temp=N;
    while(temp!=0){
      if(D[temp%10]==1){
          flag=1;
          break;
      }else{
          temp=temp/10;
      }
    }
    if(flag==1)
      N++;
  }while(flag!=0);
  
  
  printf("%d\n",N);
}
```

After:

```c
#include <stdio.h>

int bit(int x)
{
	int r = 0;
	while (x){
		r |= 1 << (x % 10);
		x /= 10;
	}
	return r;
}

int main()
{
	int N,K,b = 0;
	scanf ("%d %d",&N,&K); while (K--){
		int x; scanf ("%d",&x); b |= 1 << x;
	}

	while (bit(N)&b)N++;
	printf ("%d\n",N);

	return 0;
}
```

## Evidence

- Average coverage delta: 0.8696
- Source problems: train_AtCoder_p04039
- Rule id: `e40df3aa`

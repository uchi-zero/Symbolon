---
name: prefer-counted-for-loop-over-decrement-while
description: Use when the code consumes N symbolic inputs using a 'while(N--)' loop that mutates the loop counter, coupling termination to the symbolic input count variable.
---

# prefer-counted-for-loop-over-decrement-while

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code consumes N symbolic inputs using a 'while(N--)' loop that mutates the loop counter, coupling termination to the symbolic input count variable.

## Transformation

Rewrite the input-consumption loop as an explicit 'for(i=0;i<n;i++)' with a separate index variable, leaving the original count N untouched.

## Why it helps

An explicit indexed for-loop gives KLEE a clean, monotonic loop induction variable independent of the symbolic input N, making path conditions and loop-bound constraints simpler to solve and yielding more reusable, higher-coverage test inputs.

## Example

Before:

```c
#include <stdio.h>
int main (){
	
	int N;
	int max = 0, H, count = 0;
	scanf("%d", &N);
	
	while(N--){
		scanf("%d", &H);
		if(H >= max ){
			count++;
			max = H;
		}
	}
	
	printf("%d", count);
	
	
	
	
	
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(void)
{
  int n,i,count=0,hight=0,h;
  
  scanf("%d",&n);
  for(i=0;i<n;i++){
    scanf("%d ",&h);
    if(hight<=h){
      hight=h;
      count++;
    }
  }
  printf("%d\n",count);
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1394
- Source problems: train_AtCoder_p03072
- Rule id: `2b5652c5`

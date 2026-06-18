---
name: materialize-stream-input-into-array
description: Use when inputs are read and processed in a single online loop, interleaving scanf calls with branching logic on each value
---

# materialize-stream-input-into-array

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When inputs are read and processed in a single online loop, interleaving scanf calls with branching logic on each value

## Transformation

Split the loop into two phases: first read all N inputs into a fixed-size array, then perform the conditional/branching logic in a separate loop over the array

## Why it helps

Separating input acquisition from logic gives KLEE a flat, contiguous block of symbolic inputs to reason about, avoiding repeated interleaving of solver queries with I/O modeling and enabling the solver to consider all input values jointly when exploring branches, which yields more diverse path-covering test cases

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
int main(){
	int i,n,a=0,h=0;
	int data[20];
	scanf("%d",&n);
	for(i=0;i<n;i++){
		scanf("%d",&data[i]);
	}
	for(i=0;i<n;i++){
		if(h<=data[i]){
			a++;
			h=data[i];
		}
	}
	printf("%d",a);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1394
- Source problems: train_AtCoder_p03072
- Rule id: `2ff91084`

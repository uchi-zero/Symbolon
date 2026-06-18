---
name: ternary-instead-of-if-branch
description: Use when an accumulator is updated only inside an if-statement guarded by a comparison, creating a distinct branch in the control-flow graph
---

# ternary-instead-of-if-branch

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When an accumulator is updated only inside an if-statement guarded by a comparison, creating a distinct branch in the control-flow graph

## Transformation

Replace the conditional accumulation 'if(A[i]>B[i]) ans+=A[i]-B[i];' with a ternary expression 'sum += A[i]>B[i] ? A[i]-B[i] : 0;' that always executes a single arithmetic statement

## Why it helps

The if-statement creates an explicit branch that KLEE must fork on for every loop iteration, multiplying path counts exponentially with N; a ternary is typically lowered to a select instruction or a constant-folded expression, letting KLEE encode the choice as a single symbolic constraint instead of forking, so more inputs reach later code within the solver budget

## Example

Before:

```c
#include<stdio.h>
int main() {
	int n,A[25],B[25],i,ans=0;
	scanf("%d",&n);
	for(i=0;i<n;i++) scanf("%d",&A[i]);
	for(i=0;i<n;i++) scanf("%d",&B[i]);
	for(i=0;i<n;i++)
	  if(A[i]>B[i]) ans+=A[i]-B[i];
	printf("%d",ans);
	return 0;
}
```

After:

```c
#include <stdio.h>
int V[21], W[21];
int main(){
	int N;
	scanf("%d",&N);
	int sum=0;
	for(int i=0;i<N;i++) scanf("%d",V+i);
	for(int i=0;i<N;i++) scanf("%d",W+i);
	for(int i=0;i<N;i++) sum+=V[i]>W[i]?V[i]-W[i]:0;
	printf("%d",sum);
	
}
```

## Evidence

- Average coverage delta: 0.1833
- Source problems: train_AtCoder_p03060
- Rule id: `60e5af7b`

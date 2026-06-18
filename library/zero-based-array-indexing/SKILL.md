---
name: zero-based-array-indexing
description: Use when array is accessed with direct input values as indices without adjusting for zero-based indexing
---

# zero-based-array-indexing

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When array is accessed with direct input values as indices without adjusting for zero-based indexing

## Transformation

Subtract 1 from input values when using them as array indices to properly map 1-based user input to 0-based array indexing

## Why it helps

KLEE can generate valid array indices more easily when the symbolic values map directly to the valid index range [0,n-1] rather than requiring values in range [1,n], reducing out-of-bounds accesses and allowing more valid paths to be explored

## Example

Before:

```c
#include <stdio.h>

int main()
{
	int a[13] = {0,1,2,1,3,1,3,1,1,3,1,3,1};	
	int x,y;
//	while(){
	scanf("%d%d",&x,&y);
	printf(a[x] - a[y] ? "No" : "Yes");
	return 0;
}
```

After:

```c
#include <stdio.h>
int main(void){
  int a,b,g[12]={1,3,1,2,1,2,1,1,2,1,2,1};
  scanf("%d%d",&a,&b);
  printf("%s\n",(g[a-1]==g[b-1])? "Yes":"No");
  return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03711
- Rule id: `f71a1459`

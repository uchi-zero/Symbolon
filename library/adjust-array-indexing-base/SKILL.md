---
name: adjust-array-indexing-base
description: Use when array is accessed with direct user input as index (0-based indexing)
---

# adjust-array-indexing-base

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When array is accessed with direct user input as index (0-based indexing)

## Transformation

Subtract 1 from user input when accessing array (convert to 1-based indexing)

## Why it helps

KLEE can more easily generate valid test cases when input values start from 1 rather than 0, avoiding edge cases and improving constraint solving for array bounds

## Example

Before:

```c
#include <stdio.h>

int main(int argc, char *argv[]){
  int ary[]={0,0,2,0,1,0,1,0,0,1,0,1,0};
  
  int x, y;
  scanf("%d %d", &x, &y);
  
  printf("%s", ary[x]==ary[y]?"Yes":"No");
  
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
- Rule id: `3ca6b8c2`

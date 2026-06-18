---
name: replace-loop-search-with-explicit-branches
description: Use when code uses a loop to search for a matching value in an array and stores the index
---

# replace-loop-search-with-explicit-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a loop to search for a matching value in an array and stores the index

## Transformation

Replace the loop-based search with explicit if-else branches that directly map input values to output values

## Why it helps

Loops create path explosion in symbolic execution as KLEE must explore all iterations, while explicit branches create fewer, more direct paths with simpler constraints

## Example

Before:

```c
#include<stdio.h>
int main(void){
  char a[2];
  char b[]="ACGT";
  int i,j=0;
  scanf("%c",a);
  for(i=0;i<4;i++){
    if(a[0]==b[i]) j=i;
  }
  printf("%c\n",b[3-j]);
  return 0;
}
```

After:

```c
#include<stdio.h>
char c;
int main()
{
	c=getchar();
  if(c=='A')c='T';
  else if(c=='G')c='C';
  else if(c=='T')c='A';
  else if(c=='C')c='G';
  putchar(c);
}/**/
```

## Evidence

- Average coverage delta: 0.2249
- Source problems: train_AtCoder_p03085
- Rule id: `2a00539d`

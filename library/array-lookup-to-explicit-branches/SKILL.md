---
name: array-lookup-to-explicit-branches
description: Use when code uses a loop to search for an input value in an array and then uses the found index for further computation
---

# array-lookup-to-explicit-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a loop to search for an input value in an array and then uses the found index for further computation

## Transformation

Replace the array search loop with explicit if-else branches that directly map each possible input value to its corresponding output value

## Why it helps

KLEE can more easily explore all paths when conditions are explicit branches rather than loop iterations with array indexing, reducing the complexity of symbolic constraints

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
- Rule id: `6bbea5ea`

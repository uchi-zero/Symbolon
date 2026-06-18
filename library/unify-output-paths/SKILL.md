---
name: unify-output-paths
description: Use when multiple conditional branches have different printf/output statements with different string literals
---

# unify-output-paths

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple conditional branches have different printf/output statements with different string literals

## Transformation

Replace multiple printf statements with a single output statement by modifying a variable in each branch and outputting that variable once at the end

## Why it helps

KLEE must explore each printf with different string literals as separate paths, but when outputting a single variable, the symbolic execution can merge paths more efficiently and reduce the constraint solving complexity

## Example

Before:

```c
#include <stdio.h>
int main(void){
  char c;
  scanf("%c",&c);
  if(c=='A')printf("T");
  else if(c=='T') printf("A");
  else if(c=='C')printf("G");
  else printf("C");
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

- Average coverage delta: 0.1634
- Source problems: train_AtCoder_p03085
- Rule id: `073904b9`

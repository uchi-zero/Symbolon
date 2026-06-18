---
name: use-else-if-chain
description: Use when multiple independent if statements check the same variable for different values
---

# use-else-if-chain

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple independent if statements check the same variable for different values

## Transformation

Convert the sequence of independent if statements into an if-else-if chain

## Why it helps

Else-if chains create mutually exclusive branches that KLEE can prune early, avoiding redundant path exploration and constraint solving for impossible combinations

## Example

Before:

```c
#include<stdio.h>

int main()
{

	char b, c;

	scanf("%c", &b);

	if (b == 'A')c = 'T';
	if (b == 'T')c = 'A';
	if (b == 'C')c = 'G';
	if (b == 'G')c = 'C';







	printf("%c", c);


	return(0);
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
- Rule id: `e3ffb5c9`

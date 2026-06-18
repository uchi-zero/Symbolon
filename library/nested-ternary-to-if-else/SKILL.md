---
name: nested-ternary-to-if-else
description: Use when nested ternary operators are used for multi-way conditional branching
---

# nested-ternary-to-if-else

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested ternary operators are used for multi-way conditional branching

## Transformation

Replace nested ternary operators with sequential if-else statements, each handling one condition

## Why it helps

Sequential if-else statements create simpler path conditions for KLEE, avoiding complex nested symbolic expressions that are harder to solve and may cause path explosion or solver timeouts

## Example

Before:

```c
#include <stdio.h>
 
int main( void )
{
	char s[1+1];
	scanf("%s", s ) ;
	printf( s[0] == 'A' ? "T\n" : (s[0] == 'C' ? "G\n" : (s[0] == 'G' ? "C\n" : "A\n"))) ;
	return 0 ;
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

- Average coverage delta: 0.1839
- Source problems: train_AtCoder_p03085
- Rule id: `3bd271be`

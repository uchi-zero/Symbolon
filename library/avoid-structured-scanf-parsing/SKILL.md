---
name: avoid-structured-scanf-parsing
description: Use when scanf uses a complex format string with literal delimiters (e.g., '%lld.%lld') to parse composite numeric values into multiple integer fields
---

# avoid-structured-scanf-parsing

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When scanf uses a complex format string with literal delimiters (e.g., '%lld.%lld') to parse composite numeric values into multiple integer fields

## Transformation

Replace the structured multi-field scanf with simpler reads into floating-point variables (e.g., '%Lf') and perform arithmetic directly, optionally checking the scanf return value to handle parse failures

## Why it helps

KLEE struggles with scanf format strings containing literal separators because each delimiter introduces branching and constraints on symbolic input bytes; simpler format specifiers reduce path explosion in the parser and let KLEE focus on the program's logic, yielding test inputs that exercise more behavior when replayed

## Example

Before:

```c
#include <stdio.h>

int main(void)
{
	long long a, b, c;

	scanf("%lld %lld.%lld" ,&a, &b, &c);

	printf("%lld" ,a * (b * 100 + c) / 100);
	return 0;
}
```

After:

```c
#include<stdio.h>
#include<math.h>
int main(void)
{
  long double A,B;
  if(scanf("%Lf %Lf",&A,&B)!=0)return printf("%lld",(long long)floor(A*B)),0;
}
```

## Evidence

- Average coverage delta: 0.5
- Source problems: train_AtCoder_p02659
- Rule id: `05cdd972`

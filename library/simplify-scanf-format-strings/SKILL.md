---
name: simplify-scanf-format-strings
description: Use when scanf uses complex format strings with embedded literal delimiters (e.g., '%lld.%lld') to parse structured numeric input into multiple integer parts
---

# simplify-scanf-format-strings

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When scanf uses complex format strings with embedded literal delimiters (e.g., '%lld.%lld') to parse structured numeric input into multiple integer parts

## Transformation

Replace the multi-field structured scanf with simple whitespace-separated reads into floating-point variables, and perform arithmetic directly on the parsed values instead of reconstructing them from integer components

## Why it helps

KLEE's symbolic model of scanf struggles with literal delimiters and multi-field parsing, since matching a fixed character like '.' imposes hard constraints on symbolic input bytes that prune many paths; using simple '%Lf' reads relaxes input constraints, letting KLEE explore more program behavior and generate inputs that replay successfully on other solutions

## Example

Before:

```c
#include<stdio.h>
int main()
{
	long long a,b,c;
	scanf("%lld%lld.%lld",&a,&b,&c);
	b*=100;
	b+=c;
	a*=b;
	a/=100;
	printf("%lld",a);
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
- Rule id: `1009a984`

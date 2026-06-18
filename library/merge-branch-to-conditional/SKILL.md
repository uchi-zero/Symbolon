---
name: merge-branch-to-conditional
description: Use when if-else branches contain only simple print statements with constant strings
---

# merge-branch-to-conditional

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When if-else branches contain only simple print statements with constant strings

## Transformation

Replace if-else structure with a single printf using ternary operator (?:) to select the output string

## Why it helps

Reduces control flow complexity by eliminating explicit branches, making it easier for KLEE to reason about all paths and generate test cases that cover both outcomes

## Example

Before:

```c
#include<stdio.h>
int main()
{
    int x;
scanf("%d",&x);
if(x==7||x==5||x==3)printf("YES");
else printf("NO");
}
```

After:

```c
#include <stdio.h>

int main() {
	int a;
	scanf("%d",&a);
	printf("%s\n",(a==3||a==5||a==7)?"YES":"NO");
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1039
- Source problems: train_AtCoder_p03210
- Rule id: `7d8997fc`

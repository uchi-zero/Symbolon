---
name: add-newline-to-output
description: Use when printf statements output strings without trailing newlines
---

# add-newline-to-output

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When printf statements output strings without trailing newlines

## Transformation

Add 
 to the end of all printf format strings that don't already have one

## Why it helps

KLEE may handle output buffering differently for line-buffered vs unbuffered output, and newlines can help KLEE better distinguish between different execution paths by creating clearer output boundaries

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
#include<stdio.h>
int main(void) {
	int X;
	scanf("%d", &X);
	if (X == 3 || X == 5 || X == 7)
		printf("YES\n");
  	else 
      printf("NO");
}
```

## Evidence

- Average coverage delta: 0.1039
- Source problems: train_AtCoder_p03210
- Rule id: `b388c8c4`

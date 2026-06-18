---
name: avoid-puts-use-printf
description: Use when using puts() function for string output
---

# avoid-puts-use-printf

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When using puts() function for string output

## Transformation

Replace puts() calls with printf() calls, adding explicit newline characters where needed

## Why it helps

KLEE may have better symbolic modeling for printf() than puts(), allowing more efficient constraint solving and path exploration during symbolic execution

## Example

Before:

```c
#include <stdio.h>

int main(){
  int X;
  scanf("%d", &X);
  if(X ==3 || X==5 || X==7) puts("YES");
  else puts("NO");
  
  return 0;
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
- Rule id: `0754077b`

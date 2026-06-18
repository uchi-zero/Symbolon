---
name: use-printf-over-puts
description: Use when using puts() function for string output
---

# use-printf-over-puts

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When using puts() function for string output

## Transformation

Replace puts() calls with printf() calls, adding explicit newline characters where needed

## Why it helps

printf() has simpler symbolic execution semantics than puts(), reducing constraint complexity and allowing KLEE to explore paths more efficiently

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
- Rule id: `43f4a3f5`

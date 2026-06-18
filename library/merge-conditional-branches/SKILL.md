---
name: merge-conditional-branches
description: Use when multiple branches lead to the same output/action through separate code paths
---

# merge-conditional-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple branches lead to the same output/action through separate code paths

## Transformation

Combine the conditions into a single expression that directly produces the output

## Why it helps

Reduces the number of distinct execution paths KLEE must explore by eliminating redundant branch structures, allowing it to focus on meaningful path variations

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
- Rule id: `7b791b0e`

---
name: loop-on-string-terminator
description: Use when a loop iterates using a separately-read length variable (N) instead of the string's actual null terminator, decoupling control flow from the symbolic string contents
---

# loop-on-string-terminator

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop iterates using a separately-read length variable (N) instead of the string's actual null terminator, decoupling control flow from the symbolic string contents

## Transformation

Replace index-bounded loops over strings (i<N) with terminator-driven loops (S[i] != '\0'), and size buffers to safely hold the terminator

## Why it helps

Iterating until '\0' ties the loop bound directly to symbolic string bytes, letting KLEE fork on terminator positions and explore varied string lengths without needing consistency between N and strlen(S), reducing redundant constraints and increasing path coverage

## Example

Before:

```c
#include <stdio.h>

int main()
{
  int N,K,i;
  char S[10];

  scanf("%d",&N);
  scanf("%s",&S);
  scanf("%d",&K);

  for(i=0;i<N;i++)
  {
    if(S[i]!=S[K-1])
      S[i]='*';
  }

  printf("%s",S);

  return 0;
}
```

After:

```c
#include <stdio.h>
int main()
{
	int N, K;
	char S[11];
	scanf("%d%s%d", &N, S, &K);
	int i;
	for (i = 0; S[i] != '\0'; i++) {
		if (S[i] != S[K-1])S[i] = '*';
	}
	printf("%s\n", S);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.4687
- Source problems: train_AtCoder_p03068
- Rule id: `a66b7e78`

---
name: streaming-scan-over-buffered-string
description: Use when the program reads a whole string with scanf into a fixed buffer and then uses nested loops with array indexing to search for a pattern
---

# streaming-scan-over-buffered-string

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program reads a whole string with scanf into a fixed buffer and then uses nested loops with array indexing to search for a pattern

## Transformation

Replace buffered input plus nested-loop search with a single streaming getchar loop that maintains a small flag-based state machine tracking the search progress

## Why it helps

KLEE struggles with scanf into fixed-size buffers (creates many symbolic bytes and constrains length up front) and with nested loops over symbolic arrays (path explosion via quadratic index combinations); a linear getchar loop with a boolean state yields a single forward path per input length and simpler per-character constraints, enabling deeper exploration within the solver budget

## Example

Before:

```c
#include <stdio.h>
int main(){
	char s[105];scanf("%s",s);
	for(register int i=0;s[i];i++) if(s[i]=='C') for(register int j=i+1;s[j];j++) if(s[j]=='F') return puts("Yes"),0;
	puts("No");
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(void)
{
	int a = 0, ch;
	while((ch = getchar()) != EOF)
	{
		if (ch == 'C') a = 1;
		else if (ch == 'F' && a)
		{
			printf("Yes");
			return 0;
		}
	}
	printf("No");
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1435
- Source problems: train_AtCoder_p03957
- Rule id: `171fc313`

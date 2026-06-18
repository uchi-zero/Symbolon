---
name: bounded-loop-with-inline-check
description: Use when code uses a separate scan loop to skip a prefix and then a post-loop comparison against an unrelated bound to decide the output
---

# bounded-loop-with-inline-check

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a separate scan loop to skip a prefix and then a post-loop comparison against an unrelated bound to decide the output

## Transformation

Merge the prefix scan and bound check into a single loop bounded by the numeric input, with the branching condition (non-matching character) inlined and producing output directly inside the loop

## Why it helps

KLEE explores paths per branch; a loop bounded directly by the symbolic integer k yields concrete per-iteration branches tied to k, while a separate unbounded scan plus arithmetic comparison creates coupled constraints over loop counter and k that are harder to solve and less likely to enumerate distinct path-covering inputs

## Example

Before:

```c
#include <stdio.h>


int main()
{		
	long long n;
	int i;
	char s[111];
	
	scanf("%s%lld", s, &n);
	for(i = 0; s[i] && s[i] == 49; i++);
	if(n > i) printf("%c\n", s[i]);
	else puts("1");
				
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
    long long k;
    char s[110];
    scanf("%s%lld",s,&k);
    for(int i=0;i<k;i++){
        if(s[i]!='1'){
            putchar(s[i]);
            return 0;
        }
    }
    puts("1");
    return 0;
}
```

## Evidence

- Average coverage delta: 0.3846
- Source problems: train_AtCoder_p03282
- Rule id: `7add3fdb`

---
name: bounded-loop-with-early-exit
description: Use when code uses unbounded counting loops followed by separate comparisons against a symbolic bound, then computes the answer indirectly from the counter value
---

# bounded-loop-with-early-exit

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses unbounded counting loops followed by separate comparisons against a symbolic bound, then computes the answer indirectly from the counter value

## Transformation

Replace the count-then-compare pattern with a single loop bounded by the symbolic limit that directly emits the answer via early return when the target condition is met, falling through to the default case otherwise

## Why it helps

Bounding the loop by the symbolic input k caps path explosion at k iterations and lets KLEE branch on each character against the bound simultaneously, while early returns prune redundant suffix exploration and produce simpler per-path constraints than a counter-versus-bound comparison

## Example

Before:

```c
#include <stdio.h>

int main(void)
{
	long long int x,y=0,z,a,b,sum = 0;
	char c[101];
	for(b=0;b<100;b++) c[b] = '0';
	scanf("%s",c);
	scanf("%lld",&x);
	while(c[y] == '1') y++;
	if (y+1>x) printf("1");
	else printf("%c",c[y]);
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

- Average coverage delta: 0.4615
- Source problems: train_AtCoder_p03282
- Rule id: `138bfc95`

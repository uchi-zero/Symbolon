---
name: avoid-branch-driven-counter-resets
description: Use when code uses a conditional branch to reset/clamp a counter (e.g., `if(cnt < 0){ fill++; cnt = 0; }`) that introduces an extra control-flow path on every loop iteration
---

# avoid-branch-driven-counter-resets

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a conditional branch to reset/clamp a counter (e.g., `if(cnt < 0){ fill++; cnt = 0; }`) that introduces an extra control-flow path on every loop iteration

## Transformation

Replace the reset-and-increment pattern with a running-minimum tracker (e.g., `if(a < b) b = a;`) and compute the needed prefix/suffix counts arithmetically from the min and final values, eliminating the per-iteration state-mutating branch

## Why it helps

Each conditional reset doubles the symbolic path count per loop iteration, causing exponential path explosion in KLEE; tracking a min via a single comparison keeps the branch but avoids state divergence that compounds across iterations, allowing constraints to remain linear and solvable

## Example

Before:

```c
#include <stdio.h>

char s[105];

int main(){
    int N;
    scanf("%d", &N);
    scanf("%s", &s[1]);
    int cnt = 0, fill = 0;
    for(int i=1; i<=N; i++){
        if(s[i] == '(') cnt++;
        else cnt--;
        if(cnt < 0){ fill++; cnt = 0; }
    }
    while(fill--) printf("(");
    printf("%s", &s[1]);
    while(cnt--) printf(")");
    return 0;
}
```

After:

```c
#include<stdio.h>

#define REP(i,n) for(int i = 0; i < (n); i++)
#define STRS(i,str) for(int i = 0; str[i]; i++)

int main(void){
	char ch[120];
	int dum;
	int a = 0, b = 0;
	scanf("%d%s", &dum, ch);
	STRS(i, ch)
		if(ch[i] == '(') a++;
		else{
			a--;
			if(a < b) b = a;
		}
	REP(i, -b) putchar('(');
	printf(ch);
	REP(i, a - b) putchar(')');
	putchar('\n');
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03696
- Rule id: `e2747ca5`

---
name: per-character-symbolic-input
description: Use when code reads an entire string buffer with scanf("%s") then iterates over it, coupling all character constraints to a single symbolic array
---

# per-character-symbolic-input

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code reads an entire string buffer with scanf("%s") then iterates over it, coupling all character constraints to a single symbolic array

## Transformation

Replace bulk string input with a loop that reads one character at a time via scanf("%c", &m), processing each character immediately within the same iteration

## Why it helps

KLEE handles independent scalar symbolic variables more efficiently than indexing into a symbolic buffer; per-character reads decouple constraints, simplify pointer/array reasoning, and let the solver branch on each character independently, increasing path coverage

## Example

Before:

```c
#include <stdio.h>
int n;
int x, y;
char s[105];
int main() {
	scanf("%d", &n);
	scanf("%s", s);
	for (int i = 0; i < n; i++) {
		if (s[i] == 'B')x++;
		if (s[i] == 'R')y++;
	}
	if (y > x)printf("Yes\n");
	else printf("No\n");
}
```

After:

```c
#include<stdio.h>
int main(){
	int n,a=0,b=0;
	char m;
	scanf("%d",&n);
	getchar();
	for(int i=0;i<n;i++){
		scanf("%c",&m);
		if(m=='R'){
			a++;
		}
		else{
			b++;
		}
	}
	if(a>b){
		printf("Yes");
	}
	else{
		printf("No");
	}
}
```

## Evidence

- Average coverage delta: 0.4444
- Source problems: train_AtCoder_p03080
- Rule id: `be320c50`

---
name: split-scanf-calls
description: Use when a single scanf call mixes numeric format specifiers with a string specifier (%s) into one combined input parsing statement
---

# split-scanf-calls

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a single scanf call mixes numeric format specifiers with a string specifier (%s) into one combined input parsing statement

## Transformation

Split the combined scanf into separate calls: one for the numeric inputs and another dedicated call for the string input

## Why it helps

KLEE handles symbolic input more precisely when format specifiers are separated, since mixed-format scanf creates complex coupled constraints between integer parsing and string buffer symbolic bytes, which limits path exploration and constraint solving efficiency

## Example

Before:

```c
#include <stdio.h>
int main(){
	
	int K;
	int N;
	char S[52];
	
	scanf("%d %d %s", &N, &K, S);
	S[K-1] += 32;
	printf("%s", S);
	return 0;
}
```

After:

```c
#include <stdio.h>
int main (){
	int n,k;
	char s[60] ;
	
	scanf("%d %d",&n ,&k);
	scanf("%s",&s);
	s[k-1] += 32;
	printf("%s",s);
	
	
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03041
- Rule id: `31f555db`

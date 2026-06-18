---
name: prefer-char-by-char-input-over-string-buffer
description: Use when input is read via scanf("%s", buf) into a fixed-size array and then iterated until the null terminator
---

# prefer-char-by-char-input-over-string-buffer

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When input is read via scanf("%s", buf) into a fixed-size array and then iterated until the null terminator

## Transformation

Replace the bulk string read with a per-character read loop (e.g., while(~scanf("%c", &c))) that processes each symbolic byte as it is consumed

## Why it helps

KLEE models scanf("%s") as producing a single large symbolic buffer plus a symbolic length determined by a null terminator, creating complex constraints and many path forks on the terminator position; reading one char at a time yields simpler, independent symbolic values and a deterministic EOF-driven loop, letting KLEE enumerate input lengths and contents more efficiently.

## Example

Before:

```c
#include <stdio.h> 

char w[110];
int cnt[30];
int check;
int main(){ 
	scanf("%s",w);
	for(int i=0;w[i]!='\0';i++){
		cnt[w[i]]++;
	}
	for(int i='a';i<='z';i++){
		if(cnt[i]%2)
			check=1;
	}
	if(check)
		printf("No");
	else
		printf("Yes");
}
```

After:

```c
#include<stdio.h>
int main()
{
	char c;
	int ar[27]={0},i;
	while(~scanf("%c", &c))
		ar[c-97]++;
	for(i=0; i<26; i++)
		if(ar[i]%2)
			break;
	if(i==26) printf("Yes\n");
	else printf("No\n");
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1304
- Source problems: train_AtCoder_p04012
- Rule id: `a2bba90d`

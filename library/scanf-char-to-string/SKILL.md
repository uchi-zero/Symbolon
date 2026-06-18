---
name: scanf-char-to-string
description: Use when reading characters one-by-one using scanf("%c") in a loop
---

# scanf-char-to-string

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When reading characters one-by-one using scanf("%c") in a loop

## Transformation

Replace with a single scanf("%s") call to read the entire string at once

## Why it helps

Reading characters individually creates more complex symbolic constraints and path conditions for each iteration, while reading as a string creates simpler constraints that KLEE can solve more efficiently

## Example

Before:

```c
#include<stdio.h>
int main()
{
	char s[5];
	int i,p=0;
	for ( i = 0; i <4; i++)
	{
		scanf("%c",&s[i]);
		if(s[i]=='+')p++;
		if(s[i]=='-')p--;
	}
	printf("%d\n",p);
	return 0;
}
```

After:

```c
#include<stdio.h>
char s[4];
int t=0;
int main(){
	scanf("%s",&s);
  	for(int k=0;k<4;k++){
    	if(s[k]=='+')t++;
    	else if(s[k]=='-')t--;
    }
  	printf("%d",t);
  	return 0;
}
```

## Evidence

- Average coverage delta: 0.1282
- Source problems: train_AtCoder_p03315
- Rule id: `509969bd`

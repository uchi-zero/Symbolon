---
name: streaming-getchar-over-buffered-string
description: Use when input is read into a fixed-size buffer via scanf/fgets and then scanned with index-based loops over the array
---

# streaming-getchar-over-buffered-string

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When input is read into a fixed-size buffer via scanf/fgets and then scanned with index-based loops over the array

## Transformation

Replace buffered string input with a character-at-a-time getchar loop that updates flags and exits early upon detecting the target condition

## Why it helps

KLEE must symbolically model a fixed-size buffer with terminator constraints and reason about array indices, multiplying paths per position; a getchar loop yields one symbolic byte per iteration with simple branch conditions and allows early termination, drastically reducing path explosion and constraint complexity

## Example

Before:

```c
#include<stdio.h>
char p[110];
int main(){
    int i;
    scanf("%s",p);
    for(i=0;p[i];i++)if(p[i]=='C')break;
    for(i=i+1;p[i];i++)if(p[i]=='F')break;
    if(p[i]=='F')printf("Yes\n");
    else printf("No\n");
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
- Rule id: `dcba2176`

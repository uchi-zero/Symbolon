---
name: replace-fixed-array-with-stream-processing
description: Use when code uses fixed-size arrays with scanf to read input strings
---

# replace-fixed-array-with-stream-processing

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses fixed-size arrays with scanf to read input strings

## Transformation

Replace array-based input reading with character-by-character stream processing using getchar(), processing each character as it's read

## Why it helps

Fixed-size arrays create symbolic memory regions that KLEE must reason about holistically, while stream processing allows KLEE to explore paths incrementally per character, reducing constraint complexity and enabling earlier path pruning

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int i,b=0,c=0;
	char s[5];
	scanf("%s",s);
	for(i=0;i<3;i++)
	{
		if(s[i]=='R')c++;
		else 
		c=0;
		b=c>b?c:b;
	}
	printf("%d\n",b);
}
```

After:

```c
#include<stdio.h>
int main(void)
{
  int rainy=0,max=0,c=getchar();
  while(c=='R'||c=='S'){{if(c=='R')rainy++;else rainy=0;}max=max<rainy?rainy:max;c=getchar();}
  return printf("%d\n",max),0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p02582
- Rule id: `5fcee4d0`

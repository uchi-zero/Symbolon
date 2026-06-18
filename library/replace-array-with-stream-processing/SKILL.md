---
name: replace-array-with-stream-processing
description: Use when code uses fixed-size arrays with scanf to read input that is then processed sequentially
---

# replace-array-with-stream-processing

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses fixed-size arrays with scanf to read input that is then processed sequentially

## Transformation

Replace array storage and indexed access with character-by-character stream processing using getchar(), processing each character immediately as it's read

## Why it helps

KLEE must symbolically track array indices and bounds, creating complex path conditions; stream processing with getchar() eliminates array indexing constraints and reduces the symbolic state complexity

## Example

Before:

```c
#include <stdio.h>

int main(void)
{
	int i,total=0,max=0;
	char s[4];
	scanf("%s",s);
	for(i=0;i<3;i++){
		if(s[i]=='R')total++;
		else{
			if(total>=max)max=total;
			total=0;
		}
	}
	if(total>max)max=total;
	printf("%d",max);
	return 0;
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
- Rule id: `7d4430f8`

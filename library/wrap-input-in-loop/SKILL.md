---
name: wrap-input-in-loop
description: Use when the program reads input once and terminates after processing a single test case
---

# wrap-input-in-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program reads input once and terminates after processing a single test case

## Transformation

Wrap the input reading and processing logic in a while loop that continues until EOF (using ~scanf or similar EOF detection)

## Why it helps

KLEE can explore multiple iterations with different symbolic inputs in a single execution, generating more diverse test cases that achieve higher coverage when replayed on other solutions

## Example

Before:

```c
#include<stdio.h>
int a,b;
int main(){
  	scanf("%d %d",&a,&b);
  	if(a>b)printf("%d",a-1);
  	else printf("%d",a);
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
	int m,n;
	while(~scanf("%d%d",&m,&n)){
		if(m>n)
		printf("%d",m-1);
		else
		printf("%d\n",m);
	}
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03359
- Rule id: `543c962f`

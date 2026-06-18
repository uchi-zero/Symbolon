---
name: simplify-modulo-division-chains
description: Use when extracting middle digits using nested modulo and division operations like (x%100)/10
---

# simplify-modulo-division-chains

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When extracting middle digits using nested modulo and division operations like (x%100)/10

## Transformation

Replace (x%100)/10 with (x/10)%10 to extract middle digits

## Why it helps

The simpler (x/10)%10 pattern creates less complex symbolic expressions, making it easier for KLEE's constraint solver to reason about and generate test cases for different execution paths

## Example

Before:

```c
#include <stdio.h>

int main () {
	int a;
	scanf("%d",&a);
	if ((a%100)/10==7 || a/100==7 || (a%10)==7) {
		printf("Yes");
	}
	else printf("No");
	
	return 0;
}
```

After:

```c
#include <stdio.h>
int main (){
	int n;
	scanf ("%d",&n);
	if (n%10==7||(n/10)%10==7||n/100==7){
		printf ("Yes\n");
	}else{
		printf("No\n");
	}
}
```

## Evidence

- Average coverage delta: 0.1279
- Source problems: train_AtCoder_p02711
- Rule id: `b668b630`

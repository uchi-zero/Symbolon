---
name: split-ternary-into-if-statements
description: Use when a single output expression uses a ternary (or compound conditional) to select between distinct behaviors based on a symbolic value
---

# split-ternary-into-if-statements

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a single output expression uses a ternary (or compound conditional) to select between distinct behaviors based on a symbolic value

## Transformation

Replace the ternary with separate top-level if statements, each guarding one branch's effect on the result variable, before a single print

## Why it helps

Distinct if statements create separate branch points in the CFG that KLEE forks on independently, producing concrete test cases per condition value; a ternary inside printf is often evaluated as one constraint, reducing path/coverage diversity in generated tests

## Example

Before:

```c
#include <stdio.h>

int main(){
	int a, b;
	char op;
	
	scanf("%d %c %d", &a, &op, &b);
	printf("%d\n", op=='+'?(a+b):(a-b));
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(void)
{
	int a,c;
	char b;
	scanf("%d %c %d",&a,&b,&c);
	if(b=='-') a-=c;
	if(b=='+') a+=c;
	printf("%d\n",a);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1072
- Source problems: train_AtCoder_p03844
- Rule id: `7ac5a958`

---
name: explicit-else-if-check
description: Use when an if-else statement assumes any non-matching case falls into else branch without explicit condition check
---

# explicit-else-if-check

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When an if-else statement assumes any non-matching case falls into else branch without explicit condition check

## Transformation

Replace unconditional else branch with explicit else-if condition check for the expected alternative value

## Why it helps

KLEE can better reason about path conditions when both branches have explicit constraints, avoiding implicit assumptions about input values that may lead to missed edge cases or invalid paths

## Example

Before:

```c
#include<stdio.h>
int main(){
char S[5];
int a=0,i;
scanf("%s",S);
for(i=0;i<4;i++){
if(S[i]=='+'){a++;}
else{a--;}
}
printf("%d",a);
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
- Rule id: `56500cbc`

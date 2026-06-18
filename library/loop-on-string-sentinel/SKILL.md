---
name: loop-on-string-sentinel
description: Use when iterating over an input string using a separately-read length variable, creating a coupling constraint between the integer input and string length
---

# loop-on-string-sentinel

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When iterating over an input string using a separately-read length variable, creating a coupling constraint between the integer input and string length

## Transformation

Iterate using the string's null terminator as the loop bound (e.g., for(i=0; str[i]; i++)) and ignore the explicit length input, decoupling the two symbolic inputs

## Why it helps

KLEE must otherwise satisfy the constraint that the parsed integer matches the actual string length, pruning many feasible symbolic paths; using the sentinel lets KLEE freely vary string length independently, exploring more program behaviors

## Example

Before:

```c
#include <stdio.h>
int main(void){
  int n,i,h=0,t=0;
  char s[101];
  scanf("%d\n%s",&n,s);
  for (i=0;i<n;i++){
    if (s[i]=='(') h++;
    else{
      if (h) h--;
      else t++;
    }
  }
  for (i=0;i<t;i++) printf("(");
  printf("%s",s);
  for (i=0;i<h;i++) printf(")");
  printf("\n");
  return 0;
}
```

After:

```c
#include<stdio.h>

#define REP(i,n) for(int i = 0; i < (n); i++)
#define STRS(i,str) for(int i = 0; str[i]; i++)

int main(void){
	char ch[120];
	int dum;
	int a = 0, b = 0;
	scanf("%d%s", &dum, ch);
	STRS(i, ch)
		if(ch[i] == '(') a++;
		else{
			a--;
			if(a < b) b = a;
		}
	REP(i, -b) putchar('(');
	printf(ch);
	REP(i, a - b) putchar(')');
	putchar('\n');
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03696
- Rule id: `b75bcc89`

---
name: char-to-string-input
description: Use when reading a single character input using scanf with %c format specifier
---

# char-to-string-input

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When reading a single character input using scanf with %c format specifier

## Transformation

Replace char variable and scanf("%c") with a string buffer and scanf("%s"), then access the first character of the string

## Why it helps

String input with %s creates a more complex symbolic buffer that KLEE can explore with multiple possible values, while %c creates a single symbolic byte with limited constraint solving opportunities

## Example

Before:

```c
#include<stdio.h>

int main(){
	char x;
	scanf("%c",&x);
	if(x<'a'){
		printf("%c\n",'A');
	}
	else{
		printf("%c\n",'a');
	}
}
```

After:

```c
#include <stdio.h>

int main()
{
  char S[10]; scanf ("%s",S);
  putchar(S[0]>='a'?'a':'A');
return 0;
}
```

## Evidence

- Average coverage delta: 0.1167
- Source problems: train_AtCoder_p02627
- Rule id: `021d116c`

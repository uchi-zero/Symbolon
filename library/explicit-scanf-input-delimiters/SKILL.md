---
name: explicit-scanf-input-delimiters
description: Use when scanf reads multiple integers using a single whitespace-flexible format string like "%d%d%d%d", forcing KLEE to model arbitrary whitespace between tokens
---

# explicit-scanf-input-delimiters

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When scanf reads multiple integers using a single whitespace-flexible format string like "%d%d%d%d", forcing KLEE to model arbitrary whitespace between tokens

## Transformation

Use explicit delimiter characters between format specifiers (e.g., "%d\n%d\n%d\n%d") so symbolic input parsing has a fixed structure

## Why it helps

Generic %d parsing under symbolic execution branches on whitespace-skipping logic and token boundaries, multiplying paths inside libc; explicit newline delimiters constrain the input shape, reducing parser path explosion and letting KLEE spend its budget exploring the program logic instead

## Example

Before:

```c
#include<stdio.h>
int main(){
int a,b,c,i,j,k,x,count=0;
scanf("%d%d%d%d",&a,&b,&c,&x);
for(i=0;i<=a;i++){
for(j=0;j<=b;j++){
for(k=0;k<=c;k++){
if(i*500+j*100+k*50==x)count++;
}}}
printf("%d",count);
return 0;
}
```

After:

```c
#include<stdio.h>
int main()
{
    int a,b,c,x,i,j,k,s=0;
    scanf("%d\n%d\n%d\n%d",&a,&b,&c,&x);
    for(i=0;i<=a;i++)
        for(j=0;j<=b;j++)
        for(k=0;k<=c;k++)
    {

            if(i*500+j*100+k*50==x)s++;
    }
    printf("%d\n",s);
}
```

## Evidence

- Average coverage delta: 0.2896
- Source problems: train_AtCoder_p03448
- Rule id: `c96c149b`

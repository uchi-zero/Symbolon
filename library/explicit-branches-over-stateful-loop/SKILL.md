---
name: explicit-branches-over-stateful-loop
description: Use when the code uses a single loop with mutable state variables (like a direction flag and accumulator) that implicitly encodes multiple distinct cases, causing KLEE to explore many iterations of one path instead of distinct branches
---

# explicit-branches-over-stateful-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code uses a single loop with mutable state variables (like a direction flag and accumulator) that implicitly encodes multiple distinct cases, causing KLEE to explore many iterations of one path instead of distinct branches

## Transformation

Refactor the unified stateful loop into explicit if/else-if branches that handle each input case (e.g., s<f, s>f, wrap-around) with separate straight-line loops over concrete ranges

## Why it helps

Explicit case-splitting exposes the input-condition structure directly to KLEE's path constraints, letting it generate distinct test inputs for each branch quickly, rather than accumulating complex loop-iteration constraints over a hidden state machine

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
  int n,a,b,c,r[15],s;
  scanf("%d",&n);
  while(n--){
    scanf("%d%d",&a,&b);
    c=0;
    if(a>b)s=-1;
    else s=1;
    while(a!=b){
      r[c++]=a;
      if(a<6){
        a+=s;if(a==0)s=1;
      }
      else{
        a++;if(a>9)a=5,s=-1;
      }
    }
    for(a=0;a<c;a++){
      printf("%d ",r[a]);
    }
    printf("%d\n",b);
  }
  return 0;
}
```

After:

```c
#include<stdio.h>

int main(){

int n,s,f;
int w;

//while(1){
scanf("%d",&n);

for(int i=0;i<n;i++){
scanf("%d %d",&s,&f);

if(s<=5){
 if(s<f){
for(int i=s;i<=f;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}
     }
else if(s>f){
for(int i=s;i>=f;i--)
{
if(i!=s)printf(" ");
printf("%d",i);
}
}
}
if(s>5){
 if(f>s){
for(int i=s;i<=f;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}
}
 else if(5<f&&f<s){
	 for(int i=s;i<=9;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}

for(int i=5;i>=0;i--)
{
printf(" ");
printf("%d",i);
}

for(int i=1;i<=f;i++)
{
printf(" ");
printf("%d",i);
}

}
 else if(f<=5){
for(int i=s;i<=9;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}

for(int i=5;i>=f;i--)
{
printf(" ");
printf("%d",i);
}

}
}printf("\n");
}

return 0;
}
```

## Evidence

- Average coverage delta: 0.1819
- Source problems: train_AIZU_p00140
- Rule id: `5a75f452`

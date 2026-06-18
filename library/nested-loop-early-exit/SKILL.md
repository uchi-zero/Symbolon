---
name: nested-loop-early-exit
description: Use when nested loops have conditions that depend on computed values without explicit early termination
---

# nested-loop-early-exit

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops have conditions that depend on computed values without explicit early termination

## Transformation

Add explicit break statements when computed values exceed bounds, rather than relying on loop conditions

## Why it helps

Explicit breaks create cleaner path conditions for KLEE by avoiding symbolic execution of unnecessary iterations when bounds are exceeded

## Example

Before:

```c
#include<stdio.h>
int f(int a,int b){
int u,c=1;
for(u=0;u<b;u++)c*=a;
return c;
}
int main(){
int x,i,j,ans=1,y=0;
scanf("%d",&x);
for(i=2;i*i<=x;i++){
for(j=2;y<x;j++){
y=f(i,j);
if(y<=x&&y>ans)ans=y;
}
y=0;
}
printf("%d",ans);
return 0;

}
```

After:

```c
#include <stdio.h>
#include<math.h>
int i;
int j;
int k=2;
int a;
int max=1;
int main(){
scanf("%d",&i);
for(j=2;j<i;j++){
k=2;
while(1){
a=pow(j,k);
if(a>i)break;
if(max<a)max=a;
k++;
}
}
printf("%d",max);
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03352
- Rule id: `54f914cc`

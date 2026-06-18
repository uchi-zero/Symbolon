---
name: split-aggregated-checks-into-per-position-branches
description: Use when validation is done with a single aggregate counter loop combined with one compound boolean condition over the whole input
---

# split-aggregated-checks-into-per-position-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When validation is done with a single aggregate counter loop combined with one compound boolean condition over the whole input

## Transformation

Split the validation into separate per-region loops and individual conditional checks, each contributing to an error counter at distinct branch points

## Why it helps

Each separate if-branch creates an explicit fork in KLEE's symbolic execution tree, generating distinct test cases for each failure mode (wrong digit at each position, missing dash) instead of collapsing them into one path constraint, yielding richer test inputs with higher cross-solution coverage

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int i,a,b,cnt;
	char s[1000];
	scanf("%d%d%s",&a,&b,s);
	cnt=0;
	for(i=0;i<a+b+1;i++){
		if(s[i]!='-') cnt++;
	}
	if(s[a]=='-'&&a+b==cnt) printf("Yes\n");
	else printf("No\n");
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
int a,b,i,yj=0,x;
char s[12];
scanf("%d %d",&a,&b);
scanf("%s",s);
for(i=0;i<a;i++){
x=s[i]-'0';
if(x<0||9<x)yj++;
}
if(s[a]!='-')yj++;
for(i=a+1;i<a+b+1;i++){
x=s[i]-'0';
if(x<0||9<x)yj++;
}
if(yj==0)printf("Yes");
else printf("No");
return 0;
}
```

## Evidence

- Average coverage delta: 0.713
- Source problems: train_AtCoder_p03474
- Rule id: `92855ffd`

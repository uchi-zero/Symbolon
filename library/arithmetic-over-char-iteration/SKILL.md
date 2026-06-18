---
name: arithmetic-over-char-iteration
description: Use when loop variables iterate over character values (e.g., ASCII codes) to represent different operations or choices
---

# arithmetic-over-char-iteration

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop variables iterate over character values (e.g., ASCII codes) to represent different operations or choices

## Transformation

Replace character-based iteration with integer indices into an array that maps to the desired values or operations

## Why it helps

KLEE handles integer constraints more efficiently than character arithmetic constraints, and array indexing with simple integer bounds creates cleaner path conditions that are easier for the constraint solver to reason about

## Example

Before:

```c
#include<stdio.h>

char str[8];
int a,b,c,d;

int main(void){
	register int i,j,k,ans;
	gets(str);
	a=str[0]-'0';b=str[1]-'0';c=str[2]-'0';d=str[3]-'0';
	for(i='+';i<='-';i+=2)
		for(j='+';j<='-';j+=2)
			for(k='+';k<='-';k+=2){
				ans=a;
				if(i=='+')
					ans+=b;
				else
					ans-=b;
				if(j=='+')
					ans+=c;
				else
					ans-=c;
				if(k=='+')
					ans+=d;
				else
					ans-=d;
				if(ans==7)
					return printf("%d%c%d%c%d%c%d=7\n",a,i,b,j,c,k,d),0;
			}
	return 0;
}
```

After:

```c
#include <stdio.h>
int math[]={-1,1};
int main()
{
	int a[4];register int i,j,k;
	for(i=0; i<4; ++i)
	{
		char c=getchar();
		a[i]=c-'0';
	}
	for (i=0; i<2; ++i)
		for (j=0; j<2; ++j)
			for (k=0; k<2; ++k)
				if(a[0]+math[i]*a[1]+math[j]*a[2]+math[k]*a[3]==7)
				{
					printf("%d",a[0]);
					printf("%c",i==0?'-':'+');
					printf("%d",a[1]);
					printf("%c",j==0?'-':'+');
					printf("%d",a[2]);
					printf("%c",k==0?'-':'+');
					printf("%d",a[3]);
					printf("=7\n");
					return 0;
				}
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03545
- Rule id: `52bc8f21`

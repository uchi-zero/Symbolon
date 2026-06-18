---
name: explicit-loop-over-implicit-encoding
description: Use when loop iterations encode choices implicitly through bit manipulation or arithmetic operations on the loop counter
---

# explicit-loop-over-implicit-encoding

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop iterations encode choices implicitly through bit manipulation or arithmetic operations on the loop counter

## Transformation

Replace single loop with nested loops where each loop explicitly represents one independent choice, avoiding arithmetic decoding of the counter

## Why it helps

KLEE can directly fork execution at each explicit loop without solving complex arithmetic constraints to decode bit patterns, making path exploration more efficient

## Example

Before:

```c
#include<stdio.h>
char s[15];
int main(){
	int i,j,x,y;
	scanf("%s",s);
	for(i=0;i<8;i++){
		x=s[0]-'0';
		j=3;
		y=i;
		while(j--){
			if(y%2){
				x-=s[3-j]-'0';
			}else{
				x+=s[3-j]-'0';
			}
			y/=2;
		}
		if(x==7){
			y=i;
			printf("%c",s[0]);
			j=3;
			while(j--){
				if(y%2){
					printf("-%c",s[3-j]);
				}else{
					printf("+%c",s[3-j]);
				}
				y/=2;
			}
			printf("=7\n");
			return 0;
		}
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
- Rule id: `cd7a8002`

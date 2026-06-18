---
name: bounded-char-input-with-switch
description: Use when code reads a fixed-size string buffer with scanf("%s") and then iterates over the buffer with chained if-statements comparing characters
---

# bounded-char-input-with-switch

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code reads a fixed-size string buffer with scanf("%s") and then iterates over the buffer with chained if-statements comparing characters

## Transformation

Read input one character at a time using scanf("%1c", &c) inside the processing loop, and dispatch on the character with a switch statement instead of sequential if comparisons

## Why it helps

Per-character reads let KLEE introduce one symbolic byte per loop iteration with tight constraints rather than a large symbolic buffer of unknown effective length; switch statements produce mutually exclusive branches that the solver enumerates cleanly, avoiding redundant path explosion from independent if-tests.

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
    int n,a[11],i,j,w;
	char card[100];
	while(1){
	    scanf("%d",&n);
		if(n==0) break;
		scanf("%s",card);
		for(i=0;i<=n;i++) a[i]=0;
		j=0;
		for(i=0;i<100;i++){
		    if(card[i]=='S'){
			    a[n]=a[n]+a[j]+1;
				a[j]=0;
			}
			if(card[i]=='L'){
			    a[j]=a[j]+a[n]+1;
				a[n]=0;
			}
			if(card[i]=='M'){
			    a[j]++;
			}
			j++;
			if(j==n) j=0;
		}
		for(i=0;i<n-1;i++){
		    for(j=i+1;j<n;j++){
			    if(a[i]>a[j]){
				    w=a[i];
					a[i]=a[j];
					a[j]=w;
				}
			}
		}
		for(i=0;i<=n;i++){
		    if(i==n) printf("%d\n",a[i]);
			else printf("%d ",a[i]);
		}
	}
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(void)
{
	int N,h[10],i,n,f,a,k;
	char c;
	scanf("%d",&N);
	while(N!=0){
		for(i=0;i!=10;i++){
			h[i]=0;
		}
		f=0;
		for(i=1;i!=102;i++){
			n=i%N;
			scanf("%1c",&c);
			switch(c){
				case 'M':
				h[n]+=1;
				break;
				case 'S':
				f+=h[n]+1;
				h[n]=0;
				break;
				case 'L':
				h[n]+=f+1;
				f=0;
				break;
			}
		}
		for(i=0;i!=N;i++){
			for(a=i;a!=N;a++){
				if(h[i]>h[a]){
					k=h[i];
					h[i]=h[a];
					h[a]=k;
				}
			}
		}
		for(i=0;i!=N;i++){
			printf("%d ",h[i]);
		}
		printf("%d\n",f);
		scanf("%d",&N);
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.2192
- Source problems: train_AIZU_p00275
- Rule id: `cddca0f4`

---
name: hoist-loop-condition-to-while
description: Use when the main processing loop uses an infinite while(1) with an internal break on a sentinel input value, mixing input reading and termination logic inside the loop body
---

# hoist-loop-condition-to-while

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the main processing loop uses an infinite while(1) with an internal break on a sentinel input value, mixing input reading and termination logic inside the loop body

## Transformation

Refactor to read the sentinel input once before the loop and again at the end of each iteration, using while(n!=0) so the termination condition is explicit in the loop header

## Why it helps

An explicit loop condition lets KLEE recognize the termination predicate symbolically and prune infeasible iterations early, rather than exploring both branches of an internal break on every iteration, reducing path explosion and speeding constraint solving

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
int A[12];
int main()
{
int i,a,b,c=0,d=0,n,e=0,f=0,l=-1,j;
char S[102];
	scanf("%d",&n);
while(n!=0){
	for(i=0;i<10;i++){
	A[i]=0;
	}
	scanf("%s",&S);
	for(i=0;i<100;i++){
	l++;
	if(l==n){
	l=0;
	}
	if(S[i]=='M'){
	A[l]++;
	}
	else if(S[i]=='S') {
	f=f+A[l]+1;
	A[l]=0;
	}
	else if(S[i]=='L'){
	A[l]=A[l]+(f+1);
	f=0;
	}
	}
	l=-1;
	for(i=0;i<n;i++){
		for(j=n-1;j>=i;j--){
		if(A[i]>A[j]){
		c=A[i];
		A[i]=A[j];
		A[j]=c;
		}
		
		}
	}
	for(i=0;i<n;i++){
	printf("%d ",A[i]);
	}
	printf("%d\n",f);
	f=0;
	scanf("%d",&n);
	}
	return 0;
	
}
```

## Evidence

- Average coverage delta: 0.2216
- Source problems: train_AIZU_p00275
- Rule id: `8c164055`

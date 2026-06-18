---
name: separate-input-from-processing
description: Use when input reading is interleaved with conditional logic that updates running aggregates (e.g., tracking max while scanning), creating many symbolic branches inside the input loop
---

# separate-input-from-processing

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When input reading is interleaved with conditional logic that updates running aggregates (e.g., tracking max while scanning), creating many symbolic branches inside the input loop

## Transformation

Split the program into a pure input-collection loop that only reads values into arrays, followed by a separate processing phase (e.g., sort then select) that operates on the stored data

## Why it helps

Decoupling I/O from branching reduces the number of symbolic conditions encountered per input iteration, so KLEE forks fewer states during scanf-driven exploration and reaches deeper code with more diverse input vectors, improving coverage when the generated tests are replayed

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int n,a[21],b[101],i,z,zz,max;
	max=-1;
	scanf("%d",&n);
	for(i=0;i<n;i++){
		scanf("%d %d",&a[i],&b[i]);
		if(b[i]>max){
			max=b[i];
			z=a[i];
		}
		if(max==b[i]){
			if(z<a[i]){
			z=z;
			}
			else if(z>a[i]){
			z=a[i];
			}
		}
	}
	printf("%d %d\n",z,max);
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
	int n,i,a[20]={},v[20]={};
	int tmp;
	scanf("%d",&n);
	for(i=0;i<n;i++){
		
		scanf("%d%d",&a[i],&v[i]);
	}
	int j,h;
	for(j=0;j<n-1;j++){
		for(h=n-1;h>j;h--){
			
			if(v[h]>v[h-1]){
				tmp=v[h];
				v[h]=v[h-1];
				v[h-1]=tmp;
				tmp=a[h];
				a[h]=a[h-1];
				a[h-1]=tmp;
				
			}
		}
		
	}
	int max=v[0];
	for(i=1;i<n;i++){
		if(max==v[i]){
			if(a[0]>a[i]){
				tmp=a[0];
				a[0]=a[i];
				a[i]=tmp;
			}
		}
	}
	
	
	printf("%d %d\n",a[0],v[0]);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.7056
- Source problems: train_AIZU_p00095
- Rule id: `aeb59623`

---
name: materialize-inputs-into-arrays
description: Use when code processes inputs in a single streaming pass with intertwined conditional state updates, collapsing many input combinations into the same execution path
---

# materialize-inputs-into-arrays

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code processes inputs in a single streaming pass with intertwined conditional state updates, collapsing many input combinations into the same execution path

## Transformation

Read all inputs into arrays first, then perform the computation as explicit sort/scan loops over the stored data, separating input acquisition from decision logic

## Why it helps

Storing inputs into arrays makes each symbolic input independently constrained and exposes per-element comparisons as distinct branches (e.g., in sort and tie-breaking loops), enabling KLEE to enumerate more feasible orderings and equality cases instead of pruning paths via early combined conditionals on streaming variables

## Example

Before:

```c
#include<stdio.h>

int main(){

int x,y,n;
int X=0,Y=0;


scanf("%d",&n);

for(int i=0;i<n;i++)
{
scanf("%d %d",&x,&y);
if(i==0){Y=y;X=x;}
if(Y==y&&x<X){X=x;Y=y;}
else if(Y<y){X=x;Y=y;}

}

printf("%d %d\n",X,Y);
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

- Average coverage delta: 0.241
- Source problems: train_AIZU_p00095
- Rule id: `3dfae342`

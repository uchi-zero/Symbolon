---
name: hoist-recursion-state-to-globals
description: Use when recursive functions pass large mutable buffers and bounds via pointer/parameter chains, creating deep symbolic state that KLEE must track per call frame
---

# hoist-recursion-state-to-globals

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When recursive functions pass large mutable buffers and bounds via pointer/parameter chains, creating deep symbolic state that KLEE must track per call frame

## Transformation

Lift shared arrays and bound variables to file-scope globals and split the top-level loop so each recursive entry fixes one prefix element, reducing per-call symbolic parameters and flattening the search tree

## Why it helps

Fewer pointer parameters and smaller per-frame symbolic state shrink KLEE's path constraints and memory objects per fork, letting the solver enumerate branches faster and reach more code within the same exploration budget

## Example

Before:

```c
#include <stdio.h>
 
void output(int* list,int listnum,int num,int max);
 
int main(void) {
    int list[30];
    int n;
    while(1) {
        scanf("%d",&n);
        if(n==0)break;
        output(list,0,n,n);
    }
    return 0;
}

void output(int* list,int listnum,int num,int max) {
    int i;
    if(num<=0) {
        printf("%d",list[0]);
        for(i=1;i<listnum;i++) {
            printf(" %d",list[i]);
        }
        puts("");
    } else {
        for(i=(max>num?num:max);i>0;i--) {
            list[listnum]=i;
            output(list,listnum+1,num-i,i);
        }
    }
}
```

After:

```c
#include <stdio.h>

int na[32],n;

int narabi(int noko,int hai,int depth){
	int i;
	++depth;
//	if(hai>n)return 0;
//	printf("depth %d noko %d hai %d\n",depth,noko,hai);
	if(noko==0){
		for(i=0;i<=hai-1;i++){
			printf("%d ",na[i]);
		}
		printf("%d\n",na[hai]);
		return 0;
	}else{
		for(i=noko;i>0;i--){
			if(na[hai]<i){
				continue;
				na[hai+1]=na[hai];
				narabi(noko-na[hai],hai+1,depth);
			}else{
				na[hai+1]=i;
				narabi(noko-i,hai+1,depth);
			}
		}
	}
	return 0;
}

int main(){
	int i;
	
	while(~scanf("%d",&n)){
		if(n==0)break;
		
		for(i=n;i>0;i--){
			na[0]=i;
			narabi(n-i,0,0);
		}		
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.16
- Source problems: train_AIZU_p00430
- Rule id: `056804cd`

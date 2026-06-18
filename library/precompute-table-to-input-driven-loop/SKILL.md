---
name: precompute-table-to-input-driven-loop
description: Use when the program precomputes all answers into a static lookup table at startup and then services each input via a single array index, leaving the input variable involved in only a trivial division/index operation
---

# precompute-table-to-input-driven-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program precomputes all answers into a static lookup table at startup and then services each input via a single array index, leaving the input variable involved in only a trivial division/index operation

## Transformation

Replace the precomputation table with per-query computation that uses the symbolic input directly inside the loop bounds and arithmetic, so the input value drives branching and constraint generation

## Why it helps

KLEE's coverage and constraint-solving benefit when the symbolic input flows into branch conditions; a precomputed table reduces the input's role to a single array lookup, hiding most logic behind concrete loop iterations and producing few input-dependent paths to explore

## Example

Before:

```c
#include<stdio.h>

#define min(a, b) ((a) < (b)?(a):(b))

int ans[55], i, j, k, x;
int main(){
    for(i = 0;i <= 50;i++){
	ans[i] = 1 << 30;
    }
    for(i = 0;i <= 25;i++){
	for(j = 0;j <= 17;j++){
	    for(k = 0;k <= 10;k++){
		int p = i * 2 + j * 3 + k * 5;
		if(p > 50)continue;
		int price;
		price = (i / 5) * 4 * 380 + (i % 5) * 380;
		price += (j / 4) * 550 * 4 * 85 / 100 + (j % 4) * 550;
		price += (k / 3) * 3 * 850 * 88 / 100 + (k % 3) * 850;
		ans[p] = min(price, ans[p]);
	    }
	}
    }
    while(scanf("%d", &x), x){
	printf("%d\n", ans[x / 100]);
    }
    return 0;
}
```

After:

```c
#include<stdio.h>
int min(int a,int b){return a<b?a:b;}
int main(){
	while(1){
		int n;
		int i,j,k;
		scanf("%d",&n);
		if(n==0)return 0;
		int ans=1000000;
		int res1,res2;
		int cost1,cost2,cost3;
		for(i=0;i<(int)n/200+1;i++){
			cost1=0;
			cost2=0;
			cost3=0;
			res1=n-i*200;
			cost1=i*380-(int)(i/5)*380;
			for(j=0;j<(int)res1/300+1;j++){
				res2=res1-j*300;
				cost2=cost1+j*550-(int)(j/4)*330;
				for(k=(int)res2/500;k<(int)res2/500+1;k++){
					if(res2-k*500>0)break;
					else{
						cost3=cost2+k*850-(int)(k/3)*306;
						ans=min(ans,cost3);
					}
				}
			}
		}
		printf("%d\n",ans);
	}
}
```

## Evidence

- Average coverage delta: 0.6833
- Source problems: train_AIZU_p00106
- Rule id: `6887e350`

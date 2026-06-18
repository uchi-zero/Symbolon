---
name: avoid-large-fixed-arrays
description: Use when code uses large fixed-size arrays (e.g., box[55555]) for sieve-based algorithms or marking/filtering operations
---

# avoid-large-fixed-arrays

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses large fixed-size arrays (e.g., box[55555]) for sieve-based algorithms or marking/filtering operations

## Transformation

Replace the large array-based sieve with direct primality testing using trial division up to sqrt(n), eliminating the need for pre-allocated marking arrays

## Why it helps

Large arrays create complex memory constraints for KLEE, making it harder to reason about array accesses and increasing the symbolic execution state space, while direct computation with simple arithmetic operations produces simpler constraints

## Example

Before:

```c
#include<stdio.h>
 
int main(){
  int box[55555]={},a[55],i,j,n,cnt=0;
  scanf("%d",&n);
  for(i=2;i<55555;i++)box[i]=1;
  for(i=2;i<55555;i++){
    if(box[i]){
      if(i%10==1) a[cnt++]=i;
      for(j=i;j<55555;j+=i) box[j]=0;
    }
    if(cnt==n) break;
  }
  for(i=0;i<n;i++){
    printf("%d ",a[i]);
  }
  printf("\n");
}
```

After:

```c
#include <stdio.h>
#include <math.h>




int main()
{
	int i, j, k,n,l;
	scanf("%d", &n);
	printf("2 ");
	l = 1;
	for (i = 3;;i += 2)
	{
		k = 0;
		for (j = 3;j <= sqrt(i);j += 2)
		{
			if (i%j == 0)
			{
				k = 1;
				break;
			}
		}

		if (k == 0&&i%5==1) { printf("%d ", i); ++l;if (l == n) break; }
	}

	printf("\n");

	return 0;
}
```

## Evidence

- Average coverage delta: 0.6152
- Source problems: train_AtCoder_p03362
- Rule id: `9bb5f880`

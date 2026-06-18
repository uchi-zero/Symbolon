---
name: avoid-large-static-arrays
description: Use when code declares large static/global arrays that are accessed with symbolic indices
---

# avoid-large-static-arrays

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code declares large static/global arrays that are accessed with symbolic indices

## Transformation

Replace static array declarations with dynamic computation of values on-demand, computing array elements inline during output rather than pre-storing them

## Why it helps

Large static arrays force KLEE to track symbolic memory states for all array elements, creating complex constraints when accessed with symbolic indices, while computing values on-demand only creates constraints for the specific values actually used

## Example

Before:

```c
#include<stdio.h>
#define SZ 1001
int k, n, A[SZ][SZ];
int main() {
	int i, j;
	scanf("%d", &k);
	if (k <= 500) {
		printf("%d\n", k);
		for (i = 1; i <= k; i++) {
			for (j = 1; j <= k; j++) {
				printf("%d ", i);
			}
			printf("\n");
		}
	}
	else {
		n = 500;
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				A[i][j] = (i % 2 == 0) ? (i + j) % (n) : (i + j) % n + n;
				if (A[i][j] >= k) {
					A[i][j] -= n;
				}
			}
		}
		printf("%d\n", n);
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				printf("%d ", 1 + A[i][j]);
			}
			printf("\n");
		}
	}
}
```

After:

```c
#include<stdio.h>
int main()
{
	int k;
	scanf("%d",&k);
	if(k==1)
	{
		printf("1\n1\n");
		return 0;
	}
	int n=(((k+3)>>2)<<1);
	printf("%d\n",n);
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			int val=(i+j)%n+1;
			if(i&1)
			{
				val+=n;
				if(val>k)
				{
					val-=n;
				}
			}
			printf("%d",val);
			if(j==n-1)
			{
				putchar('\n');
			}
			else
			{
				putchar(' ');
			}
		}
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03188
- Rule id: `e5d16b03`

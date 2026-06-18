---
name: inline-conditional-computation
description: Use when conditional logic modifies pre-computed array values in a separate loop after initial computation
---

# inline-conditional-computation

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When conditional logic modifies pre-computed array values in a separate loop after initial computation

## Transformation

Merge the conditional logic directly into the initial computation loop, computing the final value in a single pass without intermediate storage

## Why it helps

Eliminates array writes and subsequent conditional array updates, reducing memory operations and allowing KLEE to reason about simpler straight-line code with fewer symbolic memory accesses

## Example

Before:

```c
#include<stdio.h>

int main() {

	int k;
	int i, j;
	int n;
	int c[500][500];

	//入力
	scanf("%d", &k);

	if (k == 1) {
		printf("1\n1\n");
		return 0;
	}

	n = (k + 3) / 4 * 2;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			c[i][j] = (i + j) % n + 1;
		}
	}
	for (i = 1; i < n; i += 2) {
		for (j = 0; j < n; j++) {
			if (c[i][j] + n <= k)c[i][j] += n;
		}
	}

	//出力
	printf("%d\n", n);
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%d ", c[i][j]);
		}
		printf("\n");
	}

	return 0;
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

- Average coverage delta: 0.438
- Source problems: train_AtCoder_p03188
- Rule id: `122747a7`

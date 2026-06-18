---
name: prefer-enumeration-over-conditional-construction
description: Use when code constructs an answer through nested conditional branches that depend on accumulated symbolic state (flags, per-index sentinels), creating many interacting path constraints
---

# prefer-enumeration-over-conditional-construction

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code constructs an answer through nested conditional branches that depend on accumulated symbolic state (flags, per-index sentinels), creating many interacting path constraints

## Transformation

Replace conditional construction with a bounded enumeration loop that iterates over all candidate outputs and checks each against the input constraints in a simple linear scan

## Why it helps

KLEE handles a single bounded loop with a straightforward equality check far better than deeply nested branches with flag variables; the linear validation produces simpler, more uniform path conditions and exposes concrete candidate values, yielding broader coverage when replayed

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int n,m,x,y;
	int a[5]={0},b[5]={0};
	scanf("%d%d",&n,&m);
	int flag=1;
	while(m--)
	{
		scanf("%d%d",&x,&y);
		if(b[x]==0||a[x]==y) {a[x]=y;b[x]=1;}
		else flag=0;
	}

	if(flag){
		if(n==1)	printf("%d\n",a[1]);
		else{
			if(a[1]==0&&b[1]==0)	a[1]=1;
			if(a[1]==0&&b[1]==1)
			{
				printf("-1\n");
				return 0;
			}
			for(int i=1;i<=n;i++)
			printf("%d",a[i]);
			printf("\n");
			}
		}
	else printf("-1\n");
	
	return 0;
 }
```

After:

```c
#include <stdio.h>
#include <math.h>

int s[11], c[11], d[11], n;

void digit(int N)
{
	int cnt = n;

	while (N)
	{
		d[cnt--] = N % 10;
		N /= 10;
	}
}

int main(void)
{
	int m, i, j;

	scanf("%d %d", &n, &m);
	for (i = 1; i <= m; i++)
		scanf("%d %d", &s[i], &c[i]);

	for (i = n > 1 ? (int)pow(10.0, n - 1.0) : 0; i < (int)pow(10.0, n); i++)
	{
		digit(i);

		for (j = 1; j <= m; j++)
			if (d[s[j]] != c[j])
				break;
		if (j == m + 1)
		{
			printf("%d", i);
			return 0;
		}
	}
	puts("-1");
	return 0;
}
```

## Evidence

- Average coverage delta: 0.5353
- Source problems: train_AtCoder_p02761
- Rule id: `8a6c4d83`

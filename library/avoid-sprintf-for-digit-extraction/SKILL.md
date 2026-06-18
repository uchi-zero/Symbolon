---
name: avoid-sprintf-for-digit-extraction
description: Use when code converts integers to strings via sprintf and compares character codes to extract digits for symbolic constraints
---

# avoid-sprintf-for-digit-extraction

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code converts integers to strings via sprintf and compares character codes to extract digits for symbolic constraints

## Transformation

Replace sprintf-based digit extraction with direct arithmetic decomposition (repeated modulo and division by 10) storing digits in an integer array, and compare integers directly instead of characters

## Why it helps

KLEE handles integer arithmetic constraints natively and efficiently, but sprintf involves complex libc string formatting routines that bloat path conditions and obscure the relationship between symbolic inputs and digit values, hindering constraint solving and path exploration

## Example

Before:

```c
#include <stdio.h>

int main()
{
	int N, M, s[6], c[6];
	scanf ("%d %d", &N, &M);
	for (int i = 0; i < M; i++) scanf ("%d %d", &s[i], &c[i]);

	int p = 1;
	for (int i = 1; i < N; i++) p *= 10;
	for (int d = p - (N == 1); d < p * 10; d++){
		char S[10];
		sprintf(S, "%d", d);
		int g = 0;
		for (int k = 0; k < M; k++){
			if (S[s[k] - 1] == c[k] + '0') g++;
		}
		if (g == M){
			puts(S);
			return 0;
		}
	}
	puts("-1");
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

- Average coverage delta: 0.694
- Source problems: train_AtCoder_p02761
- Rule id: `84be5166`

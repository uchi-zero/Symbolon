---
name: early-exit-on-search-success
description: Use when a loop searches for a satisfying condition by setting a flag and continuing to iterate over the entire space before printing the result
---

# early-exit-on-search-success

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop searches for a satisfying condition by setting a flag and continuing to iterate over the entire space before printing the result

## Transformation

Replace the flag-and-continue pattern with an immediate return/break that prints the success output as soon as the condition is met, and print the failure output after the loops

## Why it helps

Early termination prunes the symbolic execution tree by eliminating redundant path exploration after a witness is found, reducing the number of states KLEE must fork and the constraints it must solve for equivalent loop iterations

## Example

Before:

```c
#include <stdio.h>

int main () {
    int i,j,m = 0;
    int N,M,K;
    scanf("%d %d %d",&N,&M,&K);
    for (i=0;i<=N;i++){
        for (j=0;j<=M;j++){
            if (j*(N - i)+i*(M - j) == K){m=1;}
        }
    }
    if (m==1){printf("Yes\n");}
    else{printf("No\n");}
    return 0;
}
```

After:

```c
#include <stdio.h>

int main(void) {
  int i, j, n, m, k;
  scanf("%d%d%d", &n, &m, &k);
  for(i = 0; i <= n; ++i) for(j = 0; j <= m; ++j) if(i * j + (n - i) * (m - j) == k) {
    printf("Yes");
    return 0;
  }
  printf("No");
  return 0;
}
```

## Evidence

- Average coverage delta: 0.15
- Source problems: train_AtCoder_p03592
- Rule id: `f235122b`

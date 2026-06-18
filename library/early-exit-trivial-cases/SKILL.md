---
name: early-exit-trivial-cases
description: Use when code processes all inputs through a uniform loop without short-circuiting trivial input sizes (e.g., n==1), forcing KLEE to symbolically execute the full loop and array-indexing logic for degenerate cases
---

# early-exit-trivial-cases

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code processes all inputs through a uniform loop without short-circuiting trivial input sizes (e.g., n==1), forcing KLEE to symbolically execute the full loop and array-indexing logic for degenerate cases

## Transformation

Add explicit guard clauses at function entry that handle small or boundary input sizes with a direct printf and return, before any loops or array accesses

## Why it helps

Early returns prune entire subtrees of the symbolic execution tree for trivial cases, eliminating spurious path explosion through loops and reducing constraint solver work on edge-case array indices

## Example

Before:

```c
#include <stdio.h>
int main(){

  int n,i=0,lo[52]={},cnt=0;
  long long r=1;
  char sa[53],sb[53];
  scanf("%d",&n);
  scanf("%s",sa);
  scanf("%s",sb);

  for(i=0;i<n;i++){
    if(sa[i]==sb[i]){
      lo[cnt]=1;
    }else{
      lo[cnt]=2;
      i++;
    }
    cnt++;
  }
  if(lo[0]==2) r*=6;
  else r*=3;
  //for(i=0;i<cnt;i++) printf("%d",lo[i]);
  for(i=1;i<cnt;i++){
    if(lo[i-1]==1&&lo[i]==1) r*=2;
    else if(lo[i-1]==1&&lo[i]==2) r*=2;
    else if(lo[i-1]==2&&lo[i]==2) r*=3;
    r%=1000000007;
  }
  printf("%lld\n",r);

  return 0;

}
```

After:

```c
#include <stdio.h>
long long p = 1000000007;
long long ans;
char s[60];
int f[60];
int n, x, y;
int main() {
	scanf("%d", &n);
	scanf(" %s", s);
	scanf(" %s", s);
	if (n == 1) {
		printf("3\n");
		return 0;
	}
	while (x < n - 1) {
		if (s[x] == s[x + 1]) {
			f[y] = 1;
			y++;
			x += 2;
		}
		else {
			f[y] = 0;
			y++;
			x++;
		}
	}
	if (s[n - 1] != s[n - 2]) {
		f[y] = 0;
		y++;
	}
	ans = 3;
	if (f[0])ans *= 2;
	for (int i = 1; i < y; i++) {
		if (f[i - 1] == 0) {
			ans *= 2;
			ans %= p;
		}
		if (f[i] && f[i - 1]) {
			ans *= 3;
			ans %= p;
		}
	}
	printf("%lld\n", ans);
}
```

## Evidence

- Average coverage delta: 0.6842
- Source problems: train_AtCoder_p03626
- Rule id: `ce661123`

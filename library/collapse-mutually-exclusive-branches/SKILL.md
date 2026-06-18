---
name: collapse-mutually-exclusive-branches
description: Use when code uses multiple independent if-statements to check mutually exclusive values of the same variable, creating extra unnecessary branch points
---

# collapse-mutually-exclusive-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses multiple independent if-statements to check mutually exclusive values of the same variable, creating extra unnecessary branch points

## Transformation

Replace sequential independent `if (x==A) ...; if (x==B) ...;` checks with a single `if/else` structure that partitions the value space into exclusive cases

## Why it helps

KLEE forks at every branch condition; independent ifs on the same symbolic input double the path count with redundant feasibility checks, while if/else collapses them into one fork, reducing path explosion and constraint solver workload

## Example

Before:

```c
#include <stdio.h>

int main()
{
  char  s[101], tmp;
  int N, r=0, b=0;
  scanf("%d\n",&N);
  for(int i=0; i<N; i++){
    scanf("%c", &tmp);
    if(tmp=='R')r++;
    if(tmp=='B')b++;
  }
  if(r>b)printf("Yes\n");
  else printf("No\n");
}
```

After:

```c
#include<stdio.h>
int main(){
	int n,a=0,b=0;
	char m;
	scanf("%d",&n);
	getchar();
	for(int i=0;i<n;i++){
		scanf("%c",&m);
		if(m=='R'){
			a++;
		}
		else{
			b++;
		}
	}
	if(a>b){
		printf("Yes");
	}
	else{
		printf("No");
	}
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03080
- Rule id: `25dc407d`

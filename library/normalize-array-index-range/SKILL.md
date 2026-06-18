---
name: normalize-array-index-range
description: Use when code indexes a counting array using raw character values (e.g., cnt[w[i]]) and iterates over a wide ASCII range like 'a' to 'z'
---

# normalize-array-index-range

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code indexes a counting array using raw character values (e.g., cnt[w[i]]) and iterates over a wide ASCII range like 'a' to 'z'

## Transformation

Subtract a base offset so indices start at 0 (e.g., b[a[i]-'a']) and iterate the counting loop over the small normalized range, sized to fit comfortably within the array

## Why it helps

Smaller, zero-based index ranges shrink the symbolic state space and constraint expressions KLEE must solve, allowing the solver to enumerate distinct character classes faster and reach both branches of the parity check within its budget

## Example

Before:

```c
#include <stdio.h> 

char w[110];
int cnt[30];
int check;
int main(){ 
	scanf("%s",w);
	for(int i=0;w[i]!='\0';i++){
		cnt[w[i]]++;
	}
	for(int i='a';i<='z';i++){
		if(cnt[i]%2)
			check=1;
	}
	if(check)
		printf("No");
	else
		printf("Yes");
}
```

After:

```c
#include<stdio.h>
int main(){
	char a[110];
	int i, b[50]={},t=1;
	scanf("%s",a);
	for(i=0;a[i]!='\0';i++){
		b[a[i]-'a']++;
	}
	for(i=0;i<40;i++){
		if(b[i]%2!=0) t=0;
	}
	if(t==1)printf("Yes\n");
	else printf("No\n");
}
```

## Evidence

- Average coverage delta: 0.1304
- Source problems: train_AtCoder_p04012
- Rule id: `9a6876e3`

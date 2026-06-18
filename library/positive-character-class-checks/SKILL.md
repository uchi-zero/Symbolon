---
name: positive-character-class-checks
description: Use when code validates input characters by checking only for a single forbidden value (e.g., s[i]=='-') and ORs a flag, leaving the symbolic character largely unconstrained
---

# positive-character-class-checks

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code validates input characters by checking only for a single forbidden value (e.g., s[i]=='-') and ORs a flag, leaving the symbolic character largely unconstrained

## Transformation

Replace single-value negative checks with explicit positive range validation (e.g., verify digits via s[i]-'0' in [0,9]) and accumulate violations in a counter checked at the end

## Why it helps

Range-based digit constraints give KLEE tighter, more informative path conditions per byte, pruning equivalent symbolic states early and producing test inputs that exercise both valid-digit and invalid-character branches rather than just the '-' vs non-'-' split

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int n,m,flg=1,i;
	char s[11];
	scanf("%d %d",&n,&m);
	scanf("%s",s);
	for(i=0;i<n;i++){
		if(s[i]=='-') flg=0;
	}
	for(i=n+1;i<n+m+1;i++){
		if(s[i]=='-') flg=0;
	}
	if(s[n]!='-') flg=0;
	if(flg==1) printf("Yes\n");
	else printf("No\n");
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
int a,b,i,yj=0,x;
char s[12];
scanf("%d %d",&a,&b);
scanf("%s",s);
for(i=0;i<a;i++){
x=s[i]-'0';
if(x<0||9<x)yj++;
}
if(s[a]!='-')yj++;
for(i=a+1;i<a+b+1;i++){
x=s[i]-'0';
if(x<0||9<x)yj++;
}
if(yj==0)printf("Yes");
else printf("No");
return 0;
}
```

## Evidence

- Average coverage delta: 0.1574
- Source problems: train_AtCoder_p03474
- Rule id: `e9b70a17`

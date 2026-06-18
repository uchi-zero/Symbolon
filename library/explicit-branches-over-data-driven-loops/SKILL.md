---
name: explicit-branches-over-data-driven-loops
description: Use when the solution encodes problem logic via a lookup table combined with nested loops that compute results indirectly (e.g., scanning an array for matches and using modular arithmetic to traverse it)
---

# explicit-branches-over-data-driven-loops

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the solution encodes problem logic via a lookup table combined with nested loops that compute results indirectly (e.g., scanning an array for matches and using modular arithmetic to traverse it)

## Transformation

Replace the table-driven search with explicit conditional branches that enumerate the distinct input cases (e.g., s<=5 vs s>5, with sub-cases on the relation between s and f), each handled by a direct loop over the relevant numeric range

## Why it helps

KLEE forks on each branch condition, so explicit if/else cases produce well-separated path constraints tied directly to input variables, making constraint solving fast and yielding tests that cover each semantic case; data-driven loops with index arithmetic and inner while loops create deep, intertwined symbolic states whose constraints involve loop counters and modular indices, leading to path explosion or unsolved coverage of distinct input behaviors

## Example

Before:

```c
#include<stdio.h>
int L[]={0,1,2,3,4,5,6,7,8,9,5,4,3,2,1};
int main(){
	int a;
	scanf("%d",&a);
	while(a--){
		int b,c;
		scanf("%d%d",&b,&c);
		int at=0;
		int dist=99999999;
		for(int i=0;i<15;i++){
			if(L[i]==b){
				int now=i;
				int t=0;
				while(L[now]!=c){
					now++;
					now%=15;
					t++;
				}
				if(t<dist){
					dist=t;
					at=i;
				}
			}
		}
		int now=at;
		while(1){
			if(now!=at)printf(" ");
			printf("%d",L[now]);
			if(L[now]==c)break;
			now++;
			now%=15;
		}
		printf("\n");
	}
}
```

After:

```c
#include<stdio.h>

int main(){

int n,s,f;
int w;

//while(1){
scanf("%d",&n);

for(int i=0;i<n;i++){
scanf("%d %d",&s,&f);

if(s<=5){
 if(s<f){
for(int i=s;i<=f;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}
     }
else if(s>f){
for(int i=s;i>=f;i--)
{
if(i!=s)printf(" ");
printf("%d",i);
}
}
}
if(s>5){
 if(f>s){
for(int i=s;i<=f;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}
}
 else if(5<f&&f<s){
	 for(int i=s;i<=9;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}

for(int i=5;i>=0;i--)
{
printf(" ");
printf("%d",i);
}

for(int i=1;i<=f;i++)
{
printf(" ");
printf("%d",i);
}

}
 else if(f<=5){
for(int i=s;i<=9;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}

for(int i=5;i>=f;i--)
{
printf(" ");
printf("%d",i);
}

}
}printf("\n");
}

return 0;
}
```

## Evidence

- Average coverage delta: 0.3687
- Source problems: train_AIZU_p00140
- Rule id: `be0e88be`

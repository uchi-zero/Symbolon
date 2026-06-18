---
name: special-case-small-inputs-early
description: Use when a single general loop handles all input sizes including degenerate small cases that require complex symbolic constraints to navigate correctly
---

# special-case-small-inputs-early

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a single general loop handles all input sizes including degenerate small cases that require complex symbolic constraints to navigate correctly

## Transformation

Add explicit early-return branches for small/edge-case input values (e.g., n==1, n==2) with hardcoded outputs before entering the general algorithmic loop

## Why it helps

KLEE can quickly satisfy concrete equality constraints on small input values and immediately reach output paths, avoiding deep symbolic exploration of loop iterations and array index constraints needed to handle degenerate cases in the general code path

## Example

Before:

```c
#include <stdio.h>

#define N 100

int main(void){
    int dx[] = {0,1,0,-1};
    int dy[] = {-1,0,1,0};
    int map[N+2][N+2];
    int datas,d,dd,n;
    int x,y;
    
    scanf("%d", &datas);
    for (int i=0; i<datas; i++){
        // get size
        scanf("%d", &n);
        // reset map
        for (int xx=0; xx<n+2; xx++){
            for (int yy=0; yy<n+2; yy++){
                if (xx==0 || xx==n+1 || yy==0 || yy==n+1){
                    map[xx][yy]=-1;
                }else {
                    map[xx][yy]=0;
                }
            }
        }
        x=1; y=n; d=0; dd=1;
        // set spiral
        while(1){
            map[x][y]=1;
            if (map[x+dx[d]][y+dy[d]]==0){
                map[x+dx[dd]][y+dy[dd]]=-1;
            } else{
                if (map[x+dx[dd]][y+dy[dd]]==-1){
                    break;
                }else{
                    d = (d+1)%4;
                    dd = (dd+1)%4;
                }
            }
            x += dx[d];
            y += dy[d];
        }
        // output
        for (int yy=1; yy<=n; yy++){
            for (int xx=1; xx<=n; xx++){
                printf(map[xx][yy]==1?"#":" ");
            }
            printf("\n");
        }
        if (i<datas-1) printf("\n");
    }
    
    return 0;
}
```

After:

```c
#include<stdio.h>
char out[100][101];
int dx[]={0,1,0,-1};
int dy[]={-1,0,1,0};
int main(){
	int a;
	scanf("%d",&a);
	while(a--){
		int b;
		scanf("%d",&b);
		for(int i=0;i<b;i++){
			for(int j=0;j<b;j++){
				out[i][j]=' ';
			}
			out[i][b]=0;
		}
		if(b==1){
			printf("#\n");
			if(a)printf("\n");
			continue;
		}
		if(b==2){
			printf("##\n# \n");
			if(a)printf("\n");
			continue;
		}
		int row=b-1;
		int col=0;
		int dir=0;
		int len=1;
		out[row][col]='#';
		while(1){
			int nrow=row+dy[dir%4];
			int ncol=col+dx[dir%4];
			if((dir%4==0&&nrow==0)||(dir%4==3&&ncol==0)||(dir%4==2&&nrow==b-1)||(dir%4==1&&ncol==b-1)){
				row=nrow;
				col=ncol;
				out[row][col]='#';
				dir++;
				len=1;
			}else if(out[nrow+dy[dir%4]][ncol+dx[dir%4]]=='#'){
				if(len<3)break;
				dir++;
				len=1;
			}else{
				row=nrow;
				col=ncol;
				out[row][col]='#';
				len++;
			}
		}
		for(int i=0;i<b;i++)printf("%s\n",out[i]);
		if(a)printf("\n");
	}
}
```

## Evidence

- Average coverage delta: 0.2476
- Source problems: train_AIZU_p00141
- Rule id: `bb943191`

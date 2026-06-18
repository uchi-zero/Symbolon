---
name: avoid-redundant-bounds-sentinels
description: Use when the algorithm pads a 2D array with sentinel border values and uses nested initialization loops with per-cell conditionals to set them, creating extra symbolic branches per cell
---

# avoid-redundant-bounds-sentinels

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the algorithm pads a 2D array with sentinel border values and uses nested initialization loops with per-cell conditionals to set them, creating extra symbolic branches per cell

## Transformation

Replace the padded sentinel array with a tight n×n array and use natural loop bounds (x>=0 && x<n) for boundary detection, initializing all cells uniformly without per-cell branching

## Why it helps

KLEE forks on every conditional inside initialization loops; uniform initialization plus arithmetic bound checks collapse many symbolic branches into fewer, simpler path conditions, letting the solver reach the algorithm's interesting logic faster

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
int main()
{
	const int N = 100;
	char a[N][N];

	int n,d,caseCount=0,i,j,x,y,dd[]={-1,0,1,0},angle,c;
	scanf("%d",&d);
	while(d--)
	{
		scanf("%d",&n);
		for(i=0;i<n;++i)for(j=0;j<n;++j)a[i][j]=' ';

		x=0;y=n-1;
		angle=0;
		for(c=0;c<3;++c)
		{
			for(;x>=0&&y>=0&&x<n&&y<n; x+=dd[(angle+1)%4],y+=dd[angle])
			{
				a[y][x]='#';
			}
			x-=dd[(angle+1)%4];
			y-=dd[angle];
			angle=(angle+1)%4;;
		}
		if(n>2)
		{
			for(;n;)
			{
				c=0;
				for(; a[y+2*dd[angle]][x+2*dd[(angle+1)%4]]!='#'; x+=dd[(angle+1)%4],y+=dd[angle])
					a[y][x]='#',c++;
				if(c<2) break;
				angle=(angle+1)%4;;
			}
			a[y][x]='#';
		}
		else if(n==2)
			a[n-1][n-1]=' ';

		printf("%s",(caseCount++?"\n":""));
		for(i=0;i<n;++i)
		{
			for(j=0;j<n;++j)
				putchar(a[i][j]);
			putchar('\n');
		}
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.2803
- Source problems: train_AIZU_p00141
- Rule id: `a994cfed`

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
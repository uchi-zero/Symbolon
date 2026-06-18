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
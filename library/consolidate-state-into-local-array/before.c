#include<stdio.h>
int dpRR[100][100];
int dpUR[100][100];
int dpUU[100][100];
int dpRU[100][100];
int main(){
	int a,b;
	while(scanf("%d%d",&a,&b),a){
		for(int i=0;i<a;i++)dpRR[i][0]=1;
		for(int i=0;i<b;i++)dpUU[0][i]=1;
		for(int i=1;i<a;i++){
			for(int j=1;j<b;j++){
				dpUR[i][j]=dpUU[i-1][j];
				dpRR[i][j]=(dpUR[i-1][j]+dpRR[i-1][j])%100000;
				dpRU[i][j]=dpRR[i][j-1];
				dpUU[i][j]=(dpRU[i][j-1]+dpUU[i][j-1])%100000;
				//printf("%d %d: %d %d %d %d\n",i,j,dpRR[i][j],dpUR[i][j],dpUU[i][j],dpRU[i][j]);
			}
		}
		int ret=(dpUR[a-1][b-1]+dpRR[a-1][b-1]+dpUU[a-1][b-1]+dpRU[a-1][b-1])%100000;
		printf("%d\n",ret);
	}
}
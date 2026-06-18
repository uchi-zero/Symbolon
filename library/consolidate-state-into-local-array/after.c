#include <stdio.h>

int main(void){
	int i,j,k,w,h;
	while(scanf("%d%d",&w,&h),w,h){
		int dp[101][101][5]={0},ans=0;
		for(i=1;i<=h;i++) dp[i][1][4]=1;
		for(j=1;j<=w;j++) dp[1][j][4]=1;
		
		for(i=2;i<=h;i++){
			for(j=2;j<=w;j++){
				dp[i][j][0]=dp[i-1][j][3]+dp[i-1][j][4];
				dp[i][j][1]=dp[i][j-1][2]+dp[i][j-1][4];
				dp[i][j][2]=dp[i-1][j][0]+dp[i-1][j][2];
				dp[i][j][3]=dp[i][j-1][1]+dp[i][j-1][3];
				for(k=0;k<5;k++){
					if(dp[i][j][k]>=100000)dp[i][j][k]%=100000;
				}
			}
		}
		/*
		for(i=1;i<=h;i++){
			for(j=1;j<=w;j++){
				printf("(%f=%f=%f=%f=%f)%d%d\n",dp[i][j][0],dp[i][j][1],dp[i][j][2],dp[i][j][3],dp[i][j][4],i,j);
			}
		}
		*/
		for(i=0;i<5;i++){
			ans+=dp[h][w][i];
		}
		ans%=100000;
		printf("%d\n",ans);
	}
	return 0;
	
}
#include<stdio.h>
double dp[16][3][3];
char s[2];
char t[2];
char b[2];
int main(){
	int a;
	while(scanf("%d",&a),a){
		for(int i=0;i<16;i++)
			for(int j=0;j<3;j++)
				for(int k=0;k<3;k++)
					dp[i][j][k]=0;
		scanf("%s%s%s",s,t,b);
		int row=(s[0]-'A')/3;
		int col=(s[0]-'A')%3;
		int NGrow=(b[0]-'A')/3;
		int NGcol=(b[0]-'A')%3;
		dp[0][row][col]=1;
		for(int i=0;i<a;i++){
			for(int j=0;j<3;j++)
				for(int k=0;k<3;k++){
					if(j>0&&(j-1!=NGrow||k!=NGcol))dp[i+1][j-1][k]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
					if(k>0&&(j!=NGrow||k-1!=NGcol))dp[i+1][j][k-1]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
					if(j<2&&(j+1!=NGrow||k!=NGcol))dp[i+1][j+1][k]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
					if(k<2&&(j!=NGrow||k+1!=NGcol))dp[i+1][j][k+1]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
				}
		}
		int Grow=(t[0]-'A')/3;
		int Gcol=(t[0]-'A')%3;
		printf("%.8lf\n",dp[a][Grow][Gcol]);
	}
}
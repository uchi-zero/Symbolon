#include<stdio.h>
int min(int a,int b){return a<b?a:b;}
int main(){
	while(1){
		int n;
		int i,j,k;
		scanf("%d",&n);
		if(n==0)return 0;
		int ans=1000000;
		int res1,res2;
		int cost1,cost2,cost3;
		for(i=0;i<(int)n/200+1;i++){
			cost1=0;
			cost2=0;
			cost3=0;
			res1=n-i*200;
			cost1=i*380-(int)(i/5)*380;
			for(j=0;j<(int)res1/300+1;j++){
				res2=res1-j*300;
				cost2=cost1+j*550-(int)(j/4)*330;
				for(k=(int)res2/500;k<(int)res2/500+1;k++){
					if(res2-k*500>0)break;
					else{
						cost3=cost2+k*850-(int)(k/3)*306;
						ans=min(ans,cost3);
					}
				}
			}
		}
		printf("%d\n",ans);
	}
}
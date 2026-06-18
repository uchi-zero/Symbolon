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
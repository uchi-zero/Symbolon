#include <stdio.h>

int na[32],n;

int narabi(int noko,int hai,int depth){
	int i;
	++depth;
//	if(hai>n)return 0;
//	printf("depth %d noko %d hai %d\n",depth,noko,hai);
	if(noko==0){
		for(i=0;i<=hai-1;i++){
			printf("%d ",na[i]);
		}
		printf("%d\n",na[hai]);
		return 0;
	}else{
		for(i=noko;i>0;i--){
			if(na[hai]<i){
				continue;
				na[hai+1]=na[hai];
				narabi(noko-na[hai],hai+1,depth);
			}else{
				na[hai+1]=i;
				narabi(noko-i,hai+1,depth);
			}
		}
	}
	return 0;
}

int main(){
	int i;
	
	while(~scanf("%d",&n)){
		if(n==0)break;
		
		for(i=n;i>0;i--){
			na[0]=i;
			narabi(n-i,0,0);
		}		
	}
	return 0;
}
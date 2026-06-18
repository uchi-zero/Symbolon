#include<stdio.h>
int main(){
int i,a,b;
	for(i=0;i<7;i++){
	scanf("%d %d",&a,&b);
	if(b>a){
	b=b-a;
	}
	else{
	b=a-b;
	}
	printf("%d\n",b);
	}
	return 0;
}
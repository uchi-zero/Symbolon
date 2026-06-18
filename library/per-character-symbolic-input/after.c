#include<stdio.h>
int main(){
	int n,a=0,b=0;
	char m;
	scanf("%d",&n);
	getchar();
	for(int i=0;i<n;i++){
		scanf("%c",&m);
		if(m=='R'){
			a++;
		}
		else{
			b++;
		}
	}
	if(a>b){
		printf("Yes");
	}
	else{
		printf("No");
	}
}
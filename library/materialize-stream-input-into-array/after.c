#include<stdio.h>
int main(){
	int i,n,a=0,h=0;
	int data[20];
	scanf("%d",&n);
	for(i=0;i<n;i++){
		scanf("%d",&data[i]);
	}
	for(i=0;i<n;i++){
		if(h<=data[i]){
			a++;
			h=data[i];
		}
	}
	printf("%d",a);
	return 0;
}
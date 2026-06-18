#include<stdio.h>

int main(){
	char x;
	scanf("%c",&x);
	if(x<'a'){
		printf("%c\n",'A');
	}
	else{
		printf("%c\n",'a');
	}
}
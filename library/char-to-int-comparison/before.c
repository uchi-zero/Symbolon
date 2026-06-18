#include<stdio.h>
int main(){
char X,Y;
scanf("%c %c",&X,&Y);
if(X<Y){
	printf("<\n");
}else if(X>Y){
	printf(">\n");
}else{
	printf("=\n");
}


}
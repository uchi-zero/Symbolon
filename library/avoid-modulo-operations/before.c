#include<stdio.h>

int main(){
int a;scanf("%d",&a);
int L=a/100;
int R=a%100;
if(1<=L&&L<=12){
if(1<=R&&R<=12)printf("AMBIGUOUS\n");
else printf("MMYY\n");
}else{
if(1<=R&&R<=12)printf("YYMM\n");
else printf("NA\n");
}
}
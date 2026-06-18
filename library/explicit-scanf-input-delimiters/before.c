#include<stdio.h>
int main(){
int a,b,c,i,j,k,x,count=0;
scanf("%d%d%d%d",&a,&b,&c,&x);
for(i=0;i<=a;i++){
for(j=0;j<=b;j++){
for(k=0;k<=c;k++){
if(i*500+j*100+k*50==x)count++;
}}}
printf("%d",count);
return 0;
}
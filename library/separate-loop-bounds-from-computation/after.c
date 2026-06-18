#include <stdio.h>
#include<math.h>
int i;
int j;
int k=2;
int a;
int max=1;
int main(){
scanf("%d",&i);
for(j=2;j<i;j++){
k=2;
while(1){
a=pow(j,k);
if(a>i)break;
if(max<a)max=a;
k++;
}
}
printf("%d",max);
}
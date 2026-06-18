#include <stdio.h>
int main(){
int n;
scanf("%d",&n);
if(n%4==0){
printf("Yes\n");
return 0;
}
while(n>0){
if(n%7==0){
printf("Yes\n");
return 0;
}
n-=4;
}
printf("No\n");
}
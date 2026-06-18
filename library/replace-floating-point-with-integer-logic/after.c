#include<stdio.h>
#include<math.h>
int main(){
int a,b;
scanf("%d%d",&a,&b);
if(b>=100)a=a*1000+b;
else if(b>=10)a=a*100+b;
else a=a*10+b;
b=sqrt(a+0.1);
if(b*b==a)puts("Yes");
else puts("No");
return 0;
}